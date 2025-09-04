#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "YUGameModeBase.generated.h"

class AYUPlayerController;

/**
 * 
 */
UCLASS()
class YUCHAT_API AYUGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();	// 정답 번호

	bool IsGuessNumberString(const FString & InNumberString);	// 추측 번호

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
	
	void PrintChatMessageString(AYUPlayerController* InCahttingPlayerController, const FString& InChatMessageString);

	// 점수/진행 상태는 서버 권한(Authority)에서 증가시키고, 복제(Replication)로 클라에 전파
	void IncreaseGuessCount(AYUPlayerController* InChattingPlayerController);

	void ResetGame();	// 게임 끝남 -> 리셋

	void JudgeGame(AYUPlayerController* InChattingPlayerController, int InStrikeCount);
	
	
protected:
	FString SecretNumberString;	// 만든 SecretNumber 저장하기 (캐싱)

	TArray<TObjectPtr<AYUPlayerController>> AllPlayerControllers;	// 참가자들 저장
};
