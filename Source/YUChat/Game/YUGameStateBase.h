#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "YUGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class YUCHAT_API AYUGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
	
};
