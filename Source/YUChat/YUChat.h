#pragma once

#include "CoreMinimal.h"

class YUChatFunctionLibrary
{
public:
	static void MyPrintString(const AActor* InWorldContextActor, const FString& InString, float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan)
	{
		// World 단위마다 NetMode 있음
		if (IsValid(GEngine) == true && IsValid(InWorldContextActor) == true)
		{
			// 월드 단위를 얻어올 수 있을만한 Actor을 줘
			if (InWorldContextActor->GetNetMode() == NM_Client || InWorldContextActor->GetNetMode() == NM_ListenServer)
			{
				// 데디 아닌 경우에는 뷰포트에 출력
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);
			}
			// 데디 서버는 뷰포트에서 안 보여줌 -> 콘솔창에서 보이게 UE_LOG
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s"), *InString);
			}
		}
	}

	static FString GetNetModeString(const AActor* InWorldContextActor)
	{
		FString NetModeString = TEXT("None");

		if (IsValid(InWorldContextActor) == true)
		{
			// 넷모드 선언
			ENetMode NetMode = InWorldContextActor->GetNetMode();
			
			// NetMode가 뭔지에 따라 String에 넣어줌
			if (NetMode == NM_Client)
			{
				NetModeString = TEXT("Client");
			}
			else
			{
				if (NetMode == NM_Standalone)
				{
					NetModeString = TEXT("StandAlone");
				}
				else
				{
					// 리슨 or 데디 (여기는 데디)
					NetModeString = TEXT("Server");
				}
			}
		}

		return NetModeString;
	}
	
};