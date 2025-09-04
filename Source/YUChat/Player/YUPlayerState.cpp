#include "Player/YUPlayerState.h"

#include "Net/UnrealNetwork.h"	// DOREPLIFETIME

AYUPlayerState::AYUPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;	// 프로퍼티 레플리케이션 조건1
}

void AYUPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);	// ThisClass = AYUPlayerState, 속성 이름 PlayerNameString
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString AYUPlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}
