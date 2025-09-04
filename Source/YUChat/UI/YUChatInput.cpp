#include "UI/YUChatInput.h"

#include "Components/EditableTextBox.h"
#include "Player/YUPlayerController.h"

void UYUChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	// 중복 바인딩 방지
	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UYUChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UYUChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();

		if (IsValid(OwningPlayerController) == true)
		{
			// 캐스팅 성공 - OwningPlayerController → CXPlayerController 변환에 성공했다
			AYUPlayerController* OwningYUPlayerController = Cast<AYUPlayerController>(OwningPlayerController);
			if (IsValid(OwningPlayerController) == true)
			{
				OwningYUPlayerController->SetChatMessageString(Text.ToString());  // 메세지 호출
				EditableTextBox_ChatInput->SetText(FText());  // 메세지 빈 텍스트로 밀기
			}
		}
	}
}
