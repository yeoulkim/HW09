#include "Player/YUPlayerController.h"

#include "UI/YUChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/YUGameModeBase.h"
#include "YUPlayerState.h"
#include "Net/UnrealNetwork.h"	// DOREPLIFETIME
#include "YUChat.h"

AYUPlayerController::AYUPlayerController()
{
	bReplicates = true;	// 조건1
}

void AYUPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	// 여기부터는 내 로컬에서만 진행됨
	FInputModeUIOnly InputModeUIOnly;  // UI만
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UYUChatInput>(this, ChatInputWidgetClass);

		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}

		if (IsValid(NotificationTextWidgetClass) == true)
		{
			NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
			if (IsValid(NotificationTextWidgetInstance) == true)
			{
				NotificationTextWidgetInstance->AddToViewport();
			}
		}
	}
}

void AYUPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	// 들어온 채팅 메세지 대입
	ChatMessageString = InChatMessageString;

	// 아래에 정의한 함수를 setter에서 호출
	// PrintChatMessageString(ChatMessageString);
	if (IsLocalController() == true)  // OwningClient 상태
	{
		// ServerRPCPrintChatMessageString(InChatMessageString);

		AYUPlayerState* YUPS = GetPlayerState<AYUPlayerState>();
		if (IsValid(YUPS) == true)
		{
			// 본인 이름을 붙여서 (Combine) ChatMessageString 한 다음에
			// FString CombinedMessageString = YUPS->PlayerNameString + TEXT(": ") + InChatMessageString;
			FString CombinedMessageString = YUPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
			
			// 서버 쪽으로 보내~
			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void AYUPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	// 화면에 ChatMessageString 출력해줌
	// UKismetSystemLibrary::PrintString(this, ChatMessageString, true, true, FLinearColor::Red, 5.0f);

	// FString NetModeString = YUChatFunctionLibrary::GetNetModeString(this);
	// FString CombinedMessageString = FString::Printf(TEXT("%s: %s"), *NetModeString, *InChatMessageString);
	// YUChatFunctionLibrary::MyPrintString(this, CombinedMessageString, 10.f);

	YUChatFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AYUPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const	
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);	// 조건3
}

// RPC 만들 때는 함수 이름 뒤에 _Implementaion 붙여줌
// 클라이언트 -> 서버로 보내는 RPC
void AYUPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	// PrintChatMessageString(InChatMessageString);

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		AYUGameModeBase* YUGM = Cast<AYUGameModeBase>(GM);
		if (IsValid(YUGM) == true)
		{
			YUGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void AYUPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	/*
	for (TActorIterator<AYUPlayerController> It(GetWorld()); It; ++It)
	{
		AYUPlayerController* YUPlayerController = *It;
		if (IsValid(YUPlayerController) == true)
		{
			YUPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}
	*/
	PrintChatMessageString(InChatMessageString);	// 채팅 위젯/로그에 표시
}
