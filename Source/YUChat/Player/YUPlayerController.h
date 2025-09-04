#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "YUPlayerController.generated.h"

class UYUChatInput;
class UUserWidget;

/**
 * 
 */
UCLASS()
class YUCHAT_API AYUPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AYUPlayerController();	// 조건1. 생성자에서 액터가 bReplicated 지정되게 (공지사항)
	
	virtual void BeginPlay() override;

	// 채팅 문자열 설정
	void SetChatMessageString(const FString& InChatMessageString);

	// 채팅 메세지 출력
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;	// 조건3

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UYUChatInput> ChatInputWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UYUChatInput> ChatInputWidgetInstance;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidgetInstance;

	// 작성한 채팅 메세지 캐싱
	FString ChatMessageString;
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly)	// 조건2
	FText NotificationText;
	
};
