// <copyright file="TournamentManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "Player/Data/TournamentRoundData.h"
#include "Player/Data/TournamentData.h"
#include "TournamentManager.generated.h"

UCLASS(BlueprintType)
/// <summary>
/// Manages the tournament in a game instance
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UTournamentManager : public UObject
{
	GENERATED_BODY()
	
private:

	/// <summary>
	/// The parent game instance
	/// </summary>
	UGameInstance* parentGameInstance;

	/// <summary>
	/// The tournament information in which the user is participating at this moment.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UTournamentManager, meta = (AllowPrivateAccess = "true"))
	FTournamentData TournamentData;

	/// <summary>
	/// The rounds information of the tournament in which the user is participating at this moment.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UTournamentManager, meta = (AllowPrivateAccess = "true"))
	TArray<FTournamentRoundData> TournamentRounds;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="UTournamentManager"/> class.
	/// </summary>
	UTournamentManager();

	/// <summary>
	/// Gets the world
	/// </summary>
	virtual UWorld* GetWorld() const override;
	
	/// <summary>
	/// Gets the parent game instance
	/// </summary>
	UGameInstance* GetParentGameInstance();

	/// <summary>
	/// Sets the parent game instance
	/// </summary>
	virtual void SetParentGameInstance(UGameInstance* gameInstance);
};
