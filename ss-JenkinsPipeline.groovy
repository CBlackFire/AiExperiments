import groovy.json.*;
import org.eagleview.terraform.*;
import org.eagleview.util.*;
import java.nio.file.Paths;

def call(Map map) {

  def deploymentStacks = []

  def prodEnvironments = TerraformGlobals.getTerraformEnvironments().getProdEnvironments()

  def terraformAccountMap = TerraformGlobals.getTerraformEnvironments().getTerraformAccountMap()

  def orchestratorSecret = ''
  def supports_k8_steps = false

  def defaultRegion = 'us-east-2'

  String agentYaml = """
kind: Pod
metadata:
  annotations:
    iam.amazonaws.com/role: "Terraform-assume-evdata-role" # run pod in a role that is allowed to run terraform on accounts
spec:
  containers:
    - name: terraform
      image: 176992832580.dkr.ecr.us-east-2.amazonaws.com/cloudops/terraformer:${TerraformGlobals.getLatestTerraformContainerVersion()} # in platformci ecr
      command: ["cat"] # use cat to keep the container alive during jenkins build
      tty: true # required for cat"""
  
  pipeline {

    agent none

    options {
      skipDefaultCheckout true
    }

    stages {
      stage ('DisplayName') {
        agent none
        steps {
          script {
            println ("Setting build name to: ${map.displayName}")
            currentBuild.displayName = map.displayName
          }
        }
      }
      stage ('Deploy') {
        agent { kubernetes { yaml agentYaml } }
        stages {
          stage ('Checkout') {
            
            steps {
              checkout scm
            }
          }
        
          stage('Setup') {
            
            steps {
              script {
                sh "git config --global --add safe.directory ${env.WORKSPACE}"
                //Removing .git folder to avoid nested git repo problem
                sh "rm -rf .git"

                println("Git Credentials from caller: ${map.gitCredentialsId}")
                println("Detected Changesets: ${currentBuild.changeSets.size()}")
                println("Branch: ${env.BRANCH_NAME}")

                deploymentStacks = readJSON text: map.deploymentStackJson
                println("Branch: ${env.BRANCH_NAME}")

                println("-----------------------------------------------------------------")
                println("Detected Deployment Stacks:")
                println("${deploymentStacks}")
                println("Total detected deployment stacks: ${deploymentStacks.size()}.")
                println("-----------------------------------------------------------------")

                withCredentials([gitUsernamePassword(credentialsId: map.gitCredentialsId)]) {
                  for (deploymentStackMap in deploymentStacks) {
                    println("Cloning User Repos for: ${deploymentStackMap['deploymentStack']}")
                    def outputFileName = "${deploymentStackMap["serviceName"]}-${deploymentStackMap["serviceInstanceName"]}-clone-output.txt"
                    dir(deploymentStackMap["deploymentStack"]) {
                      try {
                        def tfVars = readJSON file: 'proton.auto.tfvars.json'
                        deploymentStackMap['tfVarsJson'] = tfVars
                        def inputs = tfVars['service_instance']['inputs']

                        // Load external refs if needed
                        if (inputs.containsKey('deployment_info_object')) {
                          def deploymentInfo = inputs['deployment_info_object']
                          if (deploymentInfo.containsKey('external_ref_source')) {
                            def externalRef = deploymentInfo['external_ref_source']
                              if (externalRef != null && externalRef.length() > 0) {
                                container('terraform') {

                                  withCredentials([
                                    [
                                      $class: 'AmazonWebServicesCredentialsBinding',
                                      credentialsId: terraformAccountMap[map.terraformAccount],
                                      accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                                      secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                                    ]
                                  ]) {

                                  // Transform external ref from http url to s3 uri
                                  // example https://app-sbox-1x0-s3-orchestrator-data.us-east-2.amazonaws.com/sandbox/demo-service/b05e4da4-f69c-11ed-b2e7-aced5c7fb998.json => s3://app-sbox-1x0-s3-orchestrator-data/sandbox/demo-service/b05e4da4-f69c-11ed-b2e7-aced5c7fb998.json
                                  String s3Uri = externalRef.replace('https://', 's3://').
                                    replace('.us-east-2.amazonaws.com/', '/')
                                  
                                  // Load external ref from s3
                                  println("Loading external ref from: ${s3Uri}...")
                                  sh "aws s3 cp ${s3Uri} external_ref.json --region ${defaultRegion}"
                                  println("External ref loaded.")
                                  serviceInstanceSpec = readJSON file: 'external_ref.json'
                                  println("External ref parsed.")
                                  tfVars['service_instance']['inputs'] = serviceInstanceSpec
                                  inputs = tfVars['service_instance']['inputs']
                                }
                              }
                            }
                          }
                        }

                        tee("./${outputFileName}")
                        {
                          if (inputs.containsKey('gitrepomount_map') && inputs['gitrepomount_map'].size() > 0) {
                            for(gitMount in inputs['gitrepomount_map']) {
                              def gitInfo = gitMount['object_instance']

                              gitInfo['CheckoutLocation'] = FilePathUtil.makeRelative(gitInfo['CheckoutLocation'])

                              String clonePath = gitInfo['CheckoutLocation']

                              println("Cloning User Repo ${gitInfo['GitRepoUrl']}: ${gitInfo['GitBranch']} at location ${clonePath}...")

                              try {
                                sh "rmdir -p ${clonePath}"
                              } catch (err) {}

                              sh "mkdir -p ${clonePath}"
                              println("Local folder ${clonePath} created.")
                              sh "git clone --single-branch --branch ${gitInfo['GitBranch']} ${gitInfo['GitRepoUrl']} ${clonePath}"
                              println("User Repo ${gitInfo['GitRepoUrl']}: ${gitInfo['GitBranch']} cloned.")
                            }
                          } else {
                            println("No git repo mounts found.")
                          }
                        }
                      }
                      catch (err) {
                        println ("Error while cloning repos: ${err}")
                        deploymentStackMap["temporaryError"] = err
                        deploymentStackMap["stageName"] = 'Setup'
                      }
                      finally {
                        if (fileExists("./${outputFileName}"))
                        {
                          archiveArtifacts artifacts: outputFileName
                        }
                      }
                    }
                  }
                }

                if (deploymentStacks.size() > 0)
                {
                  // Get orchestrator secret
                  println("Retrieving Orchestrator secret...")
                  container('terraform') {
                    withCredentials([
                      [
                        $class: 'AmazonWebServicesCredentialsBinding',
                        credentialsId: terraformAccountMap[map.terraformAccount],
                        accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                        secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                      ]
                    ]) {
                      String secretName = 'cops-sbox-1x0-secret-orchestrator-api-clientsecret'
                      if (map.terraformAccount == 'ci')
                      {
                        secretName = 'cops-ci-1x0-secret-orchestrator-api-clientsecret'
                      }

                      sh "aws secretsmanager get-secret-value --secret-id ${secretName} --region ${defaultRegion} > orchestrator-secret.json"
                      def orchestratorSecretJson = readJSON file: 'orchestrator-secret.json'
                      orchestratorSecret = orchestratorSecretJson['SecretString']
                      sh 'rm orchestrator-secret.json'
                      println("Orchestrator secret retrieved.")
                    }
                  }
                }
              }
            }
          }

          stage('Workload Pre-Process') {
            
            when {
              expression {
                return deploymentStacks.size() > 0
              }
            }
            steps {
              container('terraform') {
                script {
                  withCredentials([gitUsernamePassword(credentialsId: map.gitCredentialsId)]) {
                    for (deploymentStackMap in deploymentStacks) {
                      if (deploymentStackMap.containsKey("temporaryError"))
                        continue

                      def outputFileName = "${deploymentStackMap["serviceName"]}-${deploymentStackMap["serviceInstanceName"]}-semver-output.txt"
                      dir(deploymentStackMap["deploymentStack"]) {

                        println ("Processing deployment stack: '${deploymentStackMap["deploymentStack"]}'...")

                        try {
                          // Iterate through workloads
                          def inputs = deploymentStackMap['tfVarsJson']['service_instance']['inputs']
                          if (inputs['workload_map'] && inputs['workload_map'].size() > 0)
                          {
                            def updatedRepos = [:]
                            
                            inputs['workload_map'].each { workloadWrapper ->
                              def workloadObject = workloadWrapper['object_instance']
                              def workloadName = workloadWrapper['name']
                              println("Checking SemVer for workload '${workloadName}'...")

                              String targetCommit = workloadObject['target_commit']
                              String deployedCommit = workloadObject['deployed_commit']
                              def repoObject = findRepoMountObject(deploymentStackMap, workloadObject)

                              // process semver for workloads
                              if (repoObject)
                              {
                                boolean alreadyUpdated = updatedRepos.containsKey(repoObject['GitRepoUrl'])
                                boolean shouldUpdate = !alreadyUpdated && (!targetCommit || !deployedCommit || targetCommit != deployedCommit)

                                if (shouldUpdate)
                                {
                                  println("Updating SemVer for workload '${workloadName}'...")
                                  String checkOutFolder = repoObject['CheckoutLocation']
                                  String initialVersion = "1.0.0"
                                  String branchType = repoObject['semver_suffix']
                                  String githubRepo = repoObject['GitRepoUrl']
                                  String safeDirCommand = 'git config --global --add safe.directory $PWD'

                                  String resetCommand = ""
                                  if (targetCommit && targetCommit.length() > 0)
                                  {
                                    resetCommand = "git reset --hard ${targetCommit}"
                                  }

                                  tee("./${outputFileName}") {
                                    dir(checkOutFolder) {
                                      sh("""
    #!/bin/bash
    export INIT_VERSION="${initialVersion}"
    export BRANCH_TYPE="${branchType}"
    export GITHUB_TOKEN="${GIT_PASSWORD}"
    export GITHUB_REPOSITORY="${githubRepo}"
    export GIT_DISCOVERY_ACROSS_FILESYSTEM=1
    pwd

    ${safeDirCommand}
    ${resetCommand}

    python3 /pyutils/semver/updatesemver.py
    """)
                                      def semver = readFile(file: "semver.txt")
                                      println("SemVer for workload '${workloadName}': ${semver}.")  
                                      workloadObject['semver'] = semver
                                      for (workloadStepWrapper in workloadObject['steps'])
                                      {
                                        workloadStep = workloadStepWrapper['object_instance']
                                        workloadStep['semver'] = semver
                                      }
                                    }
                                  }
                                } else{
                                  println("SemVer for workload '${workloadName}' already up to date.")  
                                }
                              } else{
                                println("Skipping workload '${workloadName}' because it doesn't have an associated repo.")
                              }
                            }
                          } else{
                            println("No workloads found.  Skipping workload semver tagging.")
                          }
                        } catch (err) {
                          println ("Error while assigning semantic version: ${err}")
                          deploymentStackMap["temporaryError"] = err
                          deploymentStackMap["stageName"] = 'workload-pre-process'
                        }
                        finally {
                          if (fileExists("./${outputFileName}"))
                          {
                            archiveArtifacts artifacts: outputFileName
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }

          stage('Terraform Pre-Process') {
            
            when {
              expression {
                return deploymentStacks.size() > 0
              }
            }
            steps {
              script {
                container('terraform') {
                  // Capture evdata credentials
                  String evdata_access_key = ''
                  String evdata_secret_access_key = ''
                  String evdata_session_token = ''
                  println ("Capturing ev data aws credentials... ")
                  withCredentials([
                    [
                      $class: 'AmazonWebServicesCredentialsBinding',
                      credentialsId: terraformAccountMap["evdata"],
                      accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                      secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                    ]
                  ]) {
                    evdata_access_key = "$AWS_ACCESS_KEY_ID"
                    evdata_secret_access_key = "$AWS_SECRET_ACCESS_KEY"
                    evdata_session_token = "$AWS_SESSION_TOKEN"
                    println ("evdata credentials captured... ")
                  }

                  def schemaContent = null
                  String orc_env_access_key = ''
                  String orc_env_secret_access_key = ''
                  String orc_env_session_token = ''
                  
                  withCredentials([
                    [
                      $class: 'AmazonWebServicesCredentialsBinding',
                      credentialsId: terraformAccountMap[map.terraformAccount],
                      accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                      secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                    ]
                  ]) {
                    println ("Capturing aws orchestrator env credentials... ")
                    orc_env_access_key = "$AWS_ACCESS_KEY_ID"
                    orc_env_secret_access_key = "$AWS_SECRET_ACCESS_KEY"
                    orc_env_session_token = "$AWS_SESSION_TOKEN"
                    println ("orchestrator credentials captured... ")

                    println ("Downloading proton schema...")
                    // Download the full schema json from s3
                    sh(
                      script: "aws s3 cp ${map.protonSchemaUrl} full-schema.json --region ${defaultRegion}",
                      returnStdout: true)
                    schemaContent = readJSON file: 'full-schema.json'
                    println ("Proton schema downloaded")
                  }

                  withCredentials([gitUsernamePassword(credentialsId: 'jrun1')]) {
                    println ("Processing deployment stacks.  Count: ${deploymentStacks.size()}...")

                    for (deploymentStackMap in deploymentStacks) {
                      println ("Processing deployment stack: '${deploymentStackMap["deploymentStack"]}'...")
                      if (deploymentStackMap.containsKey("temporaryError"))
                        continue

                      withCredentials([
                        [
                          $class: 'AmazonWebServicesCredentialsBinding',
                          credentialsId: terraformAccountMap[deploymentStackMap["deploymentTarget"]],
                          accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                          secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                        ]
                      ]) {
                        dir(deploymentStackMap["deploymentStack"]) {
                          try {
                            // Assign specialized credentials to providers
                            
                            def providersFile = readFile(file: "provider.tf")
                            providersFile = providersFile
                              .replace('${evdata_access_key}', "\"${evdata_access_key}\"")
                              .replace('${evdata_secret_key}', "\"${evdata_secret_access_key}\"")
                              .replace('${evdata_session_token}', "\"${evdata_session_token}\"")
                              .replace('${orc_env_access_key}', "\"${orc_env_access_key}\"")
                              .replace('${orc_env_secret_key}', "\"${orc_env_secret_access_key}\"")
                              .replace('${orc_env_session_token}', "\"${orc_env_session_token}\"")


                            println("Writting aws specialized credentials...")
                            writeFile(file: "provider.tf", text: providersFile)

                            def moduleTemplates = [:]

                            def rmCommand = ''

                            // Fallback to avoid old variables.tf file to be interpreted as service variables.  Remove when all services are
                            // in the 7.11 or newer schema (prod).
                            if (!fileExists("input-variables.tf")) {
                              println("Fallback to avoid old variables.tf file to be interpreted as service variables.")
                              sh("mv variables.tf input-variables.tf")
                            }
                            
                            // defines whether k8 steps have already been added to the proton template
                            supports_k8_steps = fileExists("k8-workload-step.tf")

                            // Preprocess terraform dynamic modules
                            if (fileExists("var_to_local.tf")) {
                              rmCommand = rmCommand + "rm var_to_local.tf && "
                              
                              def schemaModules = schemaContent.schema.types.ModuleServiceInput.properties
                              def workloadModules = schemaContent.schema.types
                              def registeredModules = []
                              def filesToRemove = []
                              def cachedK8TemplatesMap = [:]

                              processSchemaModules(schemaModules, registeredModules, moduleTemplates, filesToRemove)
                              processSchemaModules(workloadModules, registeredModules, moduleTemplates, filesToRemove)

                              for (fileName in filesToRemove) {
                                rmCommand = rmCommand + "rm ${fileName} && "
                              }

                              rmCommand = rmCommand + 'rm proton.service_instance.variables.tf'
                              sh(rmCommand)

                              println("Creating terraform dynamic modules...")

                              // Creates a locals file for each resource
                              deploymentStackMap['tfVarsJson']['service_instance']['inputs'].each { configMap ->
                                def resourceType = configMap.key.split("_")[0].trim()

                                if (moduleTemplates.containsKey(resourceType))
                                {
                                  if (resourceType == 'workload')
                                  {
                                    createWorkloadDynamicTfModules(configMap, moduleTemplates, cachedK8TemplatesMap, map.orchestratorApiUrl, orchestratorSecret)
                                  }
                                  if (resourceType == 'variables')
                                  {
                                    createVariableDynamicTfModules(configMap, moduleTemplates)
                                  } else
                                  {
                                    createDynamicTfModules(configMap, moduleTemplates, resourceType)
                                  }
                                }
                              }
                            }

                            sh "cp ../backend.conf ./backend.conf"
                            sh """sed -i 's|terraform-proton|terraform-proton-${deploymentStackMap['serviceName']}|' ./backend.conf"""

                            // Update tfaVars
                            writeJSON file: 'proton.auto.tfvars.json', json: deploymentStackMap['tfVarsJson']
                          } catch (err) {
                            println ("Error while pre-processing terraform code: ${err}")
                            deploymentStackMap['temporaryError'] = err
                            deploymentStackMap['stageName'] = 'terraform pre-process'
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }

          stage('Plan') {
            
            when {
              expression {
                return deploymentStacks.size() > 0
              }
            }
            steps {
              container('terraform') {
                script {
                  withCredentials([gitUsernamePassword(credentialsId: 'jrun1')]) {
                    println ("Processing deployment stacks.  Count: ${deploymentStacks.size()}...")

                    for (deploymentStackMap in deploymentStacks) {
                      println ("Processing deployment stack: '${deploymentStackMap["deploymentStack"]}'...")
                      if (deploymentStackMap.containsKey("temporaryError"))
                        continue

                      withCredentials([
                        [
                          $class: 'AmazonWebServicesCredentialsBinding',
                          credentialsId: terraformAccountMap[deploymentStackMap["deploymentTarget"]],
                          accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                          secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                        ]
                      ]) {
                        dir(deploymentStackMap["deploymentStack"]) {
                          def outputFileName = "${deploymentStackMap["serviceName"]}-${deploymentStackMap["serviceInstanceName"]}-plan-output.txt"
                          try {
                            // Assign specialized credentials to providers
                            String gitCommit = ''
                            try {
                              gitCommit = "${GIT_COMMIT}"
                            } catch (err) {
                            }

                            sh "cp ../backend.conf ./backend.conf"
                            sh """sed -i 's|terraform-proton|terraform-proton-${deploymentStackMap['serviceName']}|' ./backend.conf"""

                            tee("./${outputFileName}")
                            {
                              sh "terraform init -no-color -reconfigure -backend-config=./backend.conf"
                              sh "terraform output -no-color -raw 'assumed-identity-arn'"

                              String planCommand = "terraform plan -no-color -out=tfplan " +
                                  "-var=\"commit_hash=${gitCommit}\" -var=\"platform_choice=${deploymentStackMap["deploymentTarget"]}\" "

                              String gitToken = "$GIT_PASSWORD"
                              String gitOwner = 'Infrastructure'
                              String baseUrl = 'https://github.eagleview.com'
                              String protonBranch = 'master'

                              if (map.terraformAccount == 'ci')
                              {
                                gitOwner = 'Engineering'
                                protonBranch = 'platform-non-prod'
                                if (prodEnvironments.contains(deploymentStackMap["deploymentTarget"])) {
                                  protonBranch = 'platform-prod'
                                }
                              }

                              String gitUrl = "${baseUrl}/${gitOwner}/terraform-proton-config"

                              if (supports_k8_steps) {
                                planCommand += "-var=\"orchestrator_api_url=${map.orchestratorApiUrl}\" "
                                planCommand += "-var=\"proton_service_folder=${deploymentStackMap["deploymentStack"]}\" "
                                planCommand += "-var=\"git_repo_name=terraform-proton-config\" "
                                planCommand += "-var=\"git_token=${gitToken}\" -var=\"git_url=${gitUrl}\"" +
                                  " -var=\"git_base_url=${baseUrl}\" -var=\"git_branch=${protonBranch}\"" +
                                  " -var=\"git_owner=${gitOwner}\""
                              }

                              String planCommandRedacted = planCommand.replace(gitToken, '****')
                              println("Planning deployment for: ${deploymentStackMap['deploymentStack']}.  Plan command: ${planCommandRedacted}")

                              sh planCommand
                            }
                          } catch (err) {
                            println ("Error while terraform planning: ${err}")
                            deploymentStackMap['temporaryError'] = err
                            deploymentStackMap['stageName'] = 'plan'
                          }
                          finally {
                            if (fileExists("./${outputFileName}"))
                            {
                              archiveArtifacts artifacts: outputFileName
                            }

                            if (deploymentStackMap.containsKey('temporaryError')) {
                              try {
                                sh 'rm -rf .terraform'
                              } catch (err2) {}

                              try {
                                def zipFileName = "${deploymentStackMap["serviceName"]}-${deploymentStackMap["serviceInstanceName"]}-terraform-code.zip"
                                zip zipFile: zipFileName, archive: true, dir: './'
                                archiveArtifacts artifacts: zipFileName
                              } catch (err2) {}
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }

          stage('Main Apply') {
            
            when {
              expression {
                return deploymentStacks.size() > 0
              }
            }
            steps {
              container('terraform') {
                script {
                  for (deploymentStackMap in deploymentStacks) {
                    if (deploymentStackMap.containsKey("temporaryError"))
                      continue

                    withCredentials([gitUsernamePassword(credentialsId: map.gitCredentialsId)]) {
                      withCredentials([
                        [
                          $class: 'AmazonWebServicesCredentialsBinding',
                          credentialsId: terraformAccountMap[deploymentStackMap["deploymentTarget"]],
                          accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                          secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                        ]
                      ]) {
                        def outputFileName = "${deploymentStackMap["serviceName"]}-${deploymentStackMap["serviceInstanceName"]}-apply-output.txt"
                        try {
                          tee("./${outputFileName}")
                          {
                            println("Applying deployment for: ${deploymentStackMap["deploymentStack"]}")
                            sh "terraform -chdir=${deploymentStackMap["deploymentStack"]} apply -no-color -input=false -parallelism=20 tfplan"
                          }
                        } catch (err) {
                          println ("Error while applying to terraform: ${err}")
                          deploymentStackMap['temporaryError'] = err
                          deploymentStackMap['stageName'] = 'apply'
                        } finally {
                          if (fileExists("./${outputFileName}"))
                          {
                            archiveArtifacts artifacts: outputFileName
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          stage('Workload Steps') {
            when {
              expression {
                return deploymentStacks.size() > 0
              }
            }
            steps {
              script {
                println('Starting Workload Steps')
                for (deploymentStackMap in deploymentStacks) {
                  if (deploymentStackMap.containsKey('temporaryError'))
                    continue

                  container('terraform') {
                    dir(deploymentStackMap['deploymentStack']) {
                      println("Processing service instance: ${deploymentStackMap['serviceInstanceName']}")
                      def outputFileName = "${deploymentStackMap['serviceName']}-${deploymentStackMap['serviceInstanceName']}-workload-steps-output.txt"
                      try {
                        tee("./${outputFileName}") {
                          def inputs = deploymentStackMap['tfVarsJson']['service_instance']['inputs']
                          def workloadMap = inputs['workload_map']
                          if (workloadMap) {
                            workloadMap.each { workload ->
                              def workloadName = workload['name']
                              println("Processing workload: ${workloadName}")
                              
                              def workloadFileName = "workload_${workloadName.replaceAll(' ', '_')}.json"

                              if (!fileExists(workloadFileName)) {
                                throw new Exception("Workload file name '${workloadFileName}' was not found.")
                              }
                              
                              def workloadJson = readJSON file: workloadFileName
                              println("Workload Json: ${workloadJson}")

                              boolean skipBuild = false
                              withCredentials([
                                [
                                  $class: 'AmazonWebServicesCredentialsBinding',
                                  credentialsId: terraformAccountMap[deploymentStackMap["deploymentTarget"]],
                                  accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                                  secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                                ]
                              ]) {
                                if (workloadJson.containsKey('skip_build_when')) {
                                  skipBuild = workloadJson['skip_build_when'].size() > 0
                                  println('Processing skip build conditions...')
                                  for (skipCondition in workloadJson['skip_build_when']) {
                                    condition = skipCondition['skip_condition']
                                    conditionValue = skipCondition['condition_value']
                                    println("Processing skip condition: ${condition}=${conditionValue}...")
                                    if (condition == 'image exists') {
                                      String imageExistsUri = conditionValue.trim()
                                      try
                                      {
                                        def imageTokens = imageExistsUri.split(':')
                                        String baseimageUri = imageTokens[0]
                                        String repositoryName = baseimageUri.substring(
                                          baseimageUri.indexOf('/') + 1,
                                          baseimageUri.length())

                                        String imageTag =  workloadJson['semver']
                                        if (imageTokens.size() > 1)
                                        {
                                          imageTag = imageTokens[1]
                                        }

                                        String jsonCliOutput = output = sh(
                                          returnStdout: true,
                                          script: "aws ecr list-images --repository-name ${repositoryName} --filter tagStatus=TAGGED --region ${defaultRegion} --query \"imageIds[?imageTag=='${imageTag}'].imageTag\"")
                                        
                                        def cliOutput = readJSON text: jsonCliOutput
                                        println("found images: ${cliOutput}")
                                        boolean imageFound = cliOutput.contains(imageTag)
                                        skipBuild = skipBuild && imageFound
                                        println("Condition met: ${imageFound}.")
                                      } catch (err) {
                                        println ("Error while calculating if build should be skipped based on existing image '${imageExistsUri}':  ${err}.  Build will still happen.")
                                      }
                                    }
                                  }
                                }

                                if (skipBuild) {
                                  println ("All conditions met.  Skipping build for: ${workloadName}")
                                }
                              }

                              def stepsJobInfo = [:]

                              // The git info is included in the workload to be passed to the build job
                              if (!skipBuild && inputs.containsKey('gitrepomount_map') && inputs['gitrepomount_map'].size() > 0) {
                                for (gitMount in inputs['gitrepomount_map']) {
                                  if (gitMount['name'] == workloadJson['git_repository']) {
                                    def gitInfo = gitMount['object_instance']
                                    println("Git Info: ${gitInfo}")

                                    workloadJson['repo_url'] = gitInfo['GitRepoUrl']
                                    workloadJson['branch_name'] = gitInfo['GitBranch']
                                    workloadJson['local_folder'] = gitInfo['CheckoutLocation']
                                    break
                                  }
                                }
                              }

                              workloadSteps = workloadJson['steps']
                              if (!skipBuild && workloadSteps && workloadSteps.size() > 0) {
                                // The steps are not passed to the build job
                                workloadJson['steps'] = null

                                workloadSteps.each { workloadStep ->
                                  def workloadStepName = workloadStep['name']
                                  def workloadStepType = workloadStep['object_instance']['workload_type']
                                  def enabled = true
                                  if (workloadStep['object_instance'].containsKey('enabled')) {
                                    enabled = workloadStep['object_instance']['enabled'] == 'true'
                                  }
                                  
                                  println("Processing workload step ${workloadStepName} of type ${workloadStepType}...")

                                  if (enabled && !workloadStepType.startsWith('k8')) {
                                    // Prepare the parameters to be send in the build job
                                    String workloadText = writeJSON returnText: true, json: workloadJson
                                    String workloadStepText = writeJSON returnText: true, json: workloadStep['object_instance']
                                    String stepsJobInfoText = new JsonBuilder(stepsJobInfo).toString()
                                    String jobName = "CloudOps/Orchestrator/Workload/${workloadStepType}"

                                    def jobBuild = build job: jobName, parameters: [
                                      string(name: 'ENVIRONMENT', value: deploymentStackMap["deploymentTarget"]),
                                      string(name: 'GIT_CREDENTIALS_ID', value: map.gitCredentialsId),
                                      string(name: 'WORKLOAD_SPEC_TEXT', value: workloadText),
                                      string(name:'WORKLOAD_STEP_SPEC_TEXT', value: workloadStepText),
                                      string(name:'STEPS_JOB_MAP_TEXT', value: stepsJobInfoText)]

                                    String buildNumber = jobBuild.getId()
                                    stepsJobInfo[workloadStepName] = [projectName: jobName, buildNumber: buildNumber]
                                  } else {
                                    println("Skipped workload step ${workloadStepName} of type ${workloadStepType}.")
                                  }
                                }
                              }
                            }
                          }
                        }
                      } catch (err) {
                        try {
                          println("Error processing Workload Steps: ${err}")
                          deploymentStackMap['temporaryError'] = err
                          deploymentStackMap['stageName'] = 'workload-steps'
                        } catch (err2) {
                        }
                      }
                      finally {
                        if (fileExists("./${outputFileName}")) {
                          archiveArtifacts artifacts: outputFileName
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          stage('Proton Confirmation') {
            
            when {
              expression {
                return deploymentStacks.size() > 0
              }
            }
            steps {
              container('terraform') {
                script {
                  def failed = false
                  for (deploymentStackMap in deploymentStacks) {
                    withCredentials([
                      [
                        $class: 'AmazonWebServicesCredentialsBinding',
                        credentialsId: terraformAccountMap[map.terraformAccount],
                        accessKeyVariable: 'AWS_ACCESS_KEY_ID',
                        secretKeyVariable: 'AWS_SECRET_ACCESS_KEY'
                      ]
                    ]) {
                      try
                      {
                        boolean hasTemporaryError = deploymentStackMap.containsKey('temporaryError')
                        String consoleBuildUrl = "$BUILD_URL" + "console"
                        String terraformStatus = hasTemporaryError ? "PENDING" : "SUCCEEDED"
                        String workloadStatus = hasTemporaryError ? "PENDING" : "SUCCEEDED"
                        String orchestratorStatus = "SUCCEEDED"

                        String serviceArn = "\"arn:aws:proton:us-east-2:${map.accountId}:service/${deploymentStackMap["serviceName"]}/service-instance/${deploymentStackMap["serviceInstanceName"]}\""
                        String artifactPrefix = "${deploymentStackMap["serviceName"]}-${deploymentStackMap["serviceInstanceName"]}"
                        String artifactUrl = null

                        if (hasTemporaryError) {
                          String failedStage = deploymentStackMap.containsKey('stageName') ? deploymentStackMap['stageName'] : 'unknown'
                          artifactUrl = "$BUILD_URL" + "artifact/${artifactPrefix}-${failedStage}-output.txt"

                          if (failedStage == 'apply' || failedStage == 'plan') {
                            terraformStatus = 'FAILED'
                          } else if (failedStage == 'workload pre-process') {
                            workloadStatus = 'FAILED'
                          } else if ( failedStage == 'workload-steps') {
                            terraformStatus = 'SUCCEEDED'
                            workloadStatus = 'FAILED'
                          } else {
                            orchestratorStatus = 'FAILED'
                            artifactUrl = null
                          }
                        }

                        String notifyCommand = "aws proton notify-resource-deployment-status-change --resource-arn ${serviceArn} " +
                          "--status SUCCEEDED " +
                          "--status-message \"Terraform Deployment ${terraformStatus}\" " +
                          "--region ${defaultRegion} " +
                          "--outputs " +
                            "key=jenkinsBuild,valueString=${consoleBuildUrl} " +
                            "key=terraformStatus,valueString=${terraformStatus} " + 
                            "key=workloadStatus,valueString=${workloadStatus} " + 
                            "key=orchestratorStatus,valueString=${orchestratorStatus} "

                        if (artifactUrl) {
                          notifyCommand += "key=buildArtifact,valueString=${artifactUrl} "
                        }

                        sh notifyCommand
                      } catch (err) {
                        failed = true
                        deploymentStackMap["notifyError"] = err
                      }
                    }
                  }

                  if (failed) {
                    throw new Exception("One or more services failed during deployment. Please check the Jenkins logs for more details.")
                  }

                }
              }
            }
          }
        }
      }
    }
  }
}

def createVariableDynamicTfModules(configMap, moduleTemplatesMap) {
  def moduleTemplate = moduleTemplatesMap['variables']
  println("Creating dynamic module for service variables")
  def service_variables = ''
  def locals = ''

  configMap.value.each { resource ->
    def var_name = resource['name']
    def var_value = resource['object_instance']['value']
    // We create first a local for each variable in the map.  We do this
    // because we want variables to be able to use terraform bindings instead of being
    // plain strings
    locals = locals + "var-${var_name} = \"${var_value}\"\n"
    // Then we build a map based on those locals.
    service_variables = service_variables + "${var_name} : \"" + '${local.var-' + "${var_name}}\","
  }

  // Redirect locals so they don't referencethe variables map that needs to be built based on locals.
  locals = locals.replace("local.service_variables.", "local.var-")

  moduleTemplate = moduleTemplate.replace('local_var_name = "local_var_value"', locals)
  moduleTemplate = moduleTemplate.replace('var_name : "var_value"', service_variables)

  println("Writing variables.tf file...")

  writeFile(file: "variables.tf", text: moduleTemplate)
}

@NonCPS
def getK8TemplateInstance(url, orchestratorSecret) {
  println("Sending http request to: ${url}")
  def urlObject = new URL(url);
  URLConnection connection = urlObject.openConnection();
  HttpURLConnection httpConn = (HttpURLConnection) connection;
  httpConn.setRequestProperty ("Authorization", "ClientSecret ${orchestratorSecret}");
  connection.connect();
  response = getResponseBody(connection);
  connection = null
  return response
}

def createWorkloadDynamicTfModules(configMap, moduleTemplatesMap, cachedK8TemplatesMap, orchestratorApiUrl, orchestratorSecret) {
  println("Creating dynamic modules for workloads")

  configMap.value.each { workload ->
    def workloadName = workload['name']
    workloadSteps = workload['object_instance']['steps']

    createDynamicTfModule(workload, moduleTemplatesMap, 'workload')
   
    workloadSteps.each { workloadStep ->
      def workloadStepName = workloadStep['name']
      def workloadStepType = workloadStep['object_instance']['workload_type']
      println("Processing workload step ${workloadStepName} of type ${workloadStepType}...")

      if (workloadStepType.startsWith('k8')) {
        println("Writing k8 yaml template files...")

        if (!cachedK8TemplatesMap.containsKey(workloadStepType))
        {
          response = getK8TemplateInstance("${orchestratorApiUrl}/k8templateinstance/${workloadStepType}", orchestratorSecret)

          println("Got k8 yaml template files from server.")

          k8TemplateInstances = readJSON text: response

          for (k8TemplateInstance in k8TemplateInstances['files']) {
            instanceFileName = k8TemplateInstance['name']
            fileName = "k8-template-${workloadStepType}-${instanceFileName}"
            println("Writing k8 yaml template file ${fileName}...")
            writeFile(file: fileName, text: k8TemplateInstance['content'])
          }

          cachedK8TemplatesMap['workloadStepType'] = true
        }

        createDynamicTfModule(workloadStep, moduleTemplatesMap,'k8-workload-step', workloadName)
      }
    }
  }
}

// This is NonCPS because JSON sluper is not serializable
//@NonCPS
//def parseJson(toParse) {
//  return new JsonSlurperClassic().parseText(toParse)
//}

def createDynamicTfModules(configMap, moduleTemplatesMap, resourceType) {
  configMap.value.each { resource ->
    createDynamicTfModule(resource, moduleTemplatesMap, resourceType)
  }
}

def createDynamicTfModule(resource, moduleTemplatesMap, resourceType, resourcePrefix = null) {
  def moduleTemplate = moduleTemplatesMap[resourceType]

  def newResourceName = ''

  if (resourcePrefix) {
    newResourceName = "${resourceType}_${resourcePrefix}_${resource['name']}"
  } else {
    newResourceName = "${resourceType}_${resource['name']}"
  }

  // TODO: This needs a regex to replace invalid characters with "_".  Not just spaces.
  newResourceName = newResourceName.replace(" ", "_")

  println("Creating dynamic module for resource: ${newResourceName}...")

  def protonVars = new JsonBuilder(resource['object_instance']).toPrettyString()

  def toWriteLocals = "locals { ${newResourceName} = ${protonVars} }"
  writeFile(file: "${newResourceName}-locals.tf", text: toWriteLocals)
  //println("locals: ${toWriteLocals}")

  def toWriteModule = moduleTemplate.
    replace("locals { ${resourceType}", "locals { ${newResourceName}").
    replace("local.${resourceType}", "local.${newResourceName}").
    replace("module \"${resourceType}", "module \"${newResourceName}").
    replace("module.${resourceType}", "module.${newResourceName}").
    replace("resource \"null_resource\" \"${resourceType}", "resource \"null_resource\" \"${newResourceName}").
    replace("null_resource.${resourceType}", "null_resource.${newResourceName}").
    replace("resource \"local_file\" \"${resourceType}", "resource \"local_file\" \"${newResourceName}").
    replace("local_file.${resourceType}", "local_file.${newResourceName}").
    replace("data \"http\" \"${resourceType}", "data \"http\" \"${newResourceName}").
    replace("data.http.${resourceType}", "data.http.${newResourceName}").
    replace('$${resource-name}', newResourceName)

  writeFile(file: "${newResourceName}-module.tf", text: toWriteModule)
}

def processSchemaModules(schemaModules, registeredModules, moduleTemplates, filesToRemove) {
  // Delete tf placeholders for modules.
  schemaModules.each { key, module ->
    print("Analizing schema type ${key}...")
    def resourceType = module['x-resourceType']
    if (resourceType) {
      def fileName = "${resourceType}.tf"
      registeredModules.add(resourceType)
      boolean fileExists = fileExists(fileName)
      boolean moduleAlreadyRegisterd = moduleTemplates.containsKey(resourceType)
      if (fileExists && !moduleAlreadyRegisterd) {
        def templateFile = readFile(file: fileName)
        moduleTemplates[resourceType] = templateFile
        filesToRemove.add(fileName)
        print("Schema Type ${key} registered.")
      } else
      {
        println("Skipping ${resourceType} schema module registration because: file exists = '${fileExists}' or module already exists = '${moduleAlreadyRegisterd}'")
      }
    }
  }
}

@NonCPS
def getResponseBody(HttpURLConnection connection) {
  int responseCode = connection.getResponseCode();
  if (responseCode != 200) {
    throw new RuntimeException("Error retrieving body response: ${responseCode}");
  }

  BufferedReader br = null;
  StringBuilder body = null;
  String line = "";
  try {
    br = new BufferedReader(new InputStreamReader(connection.getInputStream()));
    body = new StringBuilder();
    while ((line = br.readLine()) != null)
        body.append(line);
    return body.toString();
  } catch (Exception e) {
      throw new RuntimeException(e);
  }
}

def findRepoMountObject(deploymentStackMap, workloadObject) {
  repoName = workloadObject['git_repository']

  def inputs = deploymentStackMap['tfVarsJson']['service_instance']['inputs']
  if (inputs.containsKey('gitrepomount_map') && inputs['gitrepomount_map'].size() > 0) {
    for(gitMount in inputs['gitrepomount_map']) {
      if (gitMount['name'] == repoName) {
        return gitMount['object_instance']
      }
    }
  }

  return null
}
