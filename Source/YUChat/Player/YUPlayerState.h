#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "YUPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class YUCHAT_API AYUPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AYUPlayerState();	// 프로퍼티 레플리케이션 조건1. 생성자에서 액터 속성 설정하기

	// 조건3. 함수 통해서 해당 네트워크가 복제될 것을 명시
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString();	// 플레이어 이름 & 현재 GuessCount 알려 주기
	
public:
	UPROPERTY(Replicated)	// 조건2. 네트워크로 복제한 액터의 속성을 키워드로 지정
	FString PlayerNameString;	// 플레이어 번호

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;
	
};
