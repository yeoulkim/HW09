#include "Game/YUGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/YUPlayerController.h"

void AYUGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			AYUPlayerController* YUPC = Cast<AYUPlayerController>(PC);
			if (IsValid(YUPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				YUPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
