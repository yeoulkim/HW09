#include "Game/YUGameModeBase.h"

#include "YUGameStateBase.h"
#include "Player/YUPlayerController.h"
#include "EngineUtils.h"
#include "Player/YUPlayerState.h"

void AYUGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 한 번만 정답 번호 만들기 -> GenerateSecretNumber()
	// 캐싱하기 -> SecretNumberString
	SecretNumberString = GenerateSecretNumber();
}

// 로그인 후에 호출되는 함수
void AYUGameModeBase::OnPostLogin(AController* NewPlayer)  // 컨트롤러 넣기
{
	Super::OnPostLogin(NewPlayer);

	// 들어오는 걸 YUPlayerController로 형 변환
	AYUPlayerController* YUPlayerController = Cast<AYUPlayerController>(NewPlayer);
	if (IsValid(YUPlayerController) == true)
	{
		// PlayerController의 NotificationText 를 수정 : 너 로그인 했어~
		YUPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		// 이 YUPlayerController는 서버에 있음 -> 서버에 있는 애의 property를 수정해줌
		// YUPlayerController의 생성자에서 bReplicates = true로 했기 때문에 이 PlayerController는 복제됨
		// 조건 3개 충족해서 NotificationText가 Client에도 복제됨

		AllPlayerControllers.Add(YUPlayerController);

		AYUPlayerState* YUPS = YUPlayerController->GetPlayerState<AYUPlayerState>();
		if (IsValid(YUPS) == true)
		{
			YUPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		AYUGameStateBase* YUGameStateBase = GetGameState<AYUGameStateBase>();
		if (IsValid(YUGameStateBase) == true)
		{
			YUGameStateBase->MulticastRPCBroadcastLoginMessage(YUPS->PlayerNameString);
		}
	}
}

// 중복 없는 랜덤 숫자 3개 만들기
FString AYUGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());	// 실행마다 다른 랜덤값
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });	// 배열 필터링(양수만 남기기)

	FString Result;	// 결과 담을 빈 문자열 변수 생성
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);	// 남이있는 숫자 중에서 랜덤 인덱스 뽑음
		Result.Append(FString::FromInt(Numbers[Index]));	// 뽑은 숫자를 문자열에 붙임
		Numbers.RemoveAt(Index);	// 이미 뽑은 숫자는 제거해서 중복 방지하기
	}
	return Result;
}

bool AYUGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;  // TSet에 넣어서 중복 자동 제거
		for (TCHAR C : InNumberString)  // 문자?
		{
			if (FChar::IsDigit(C) == false || C == '0')  // 반드시 숫자, 0은 허용 안함
			{
				bIsUnique = false;
				break;  // 나가
			}
			
			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)  // 중복 여부 확인
		{
			break;
		}

		bCanPlay = true; // 3자리, 모두 숫자, 중복 없음 -> 유효한 숫자
		
	} while (false);	

	return bCanPlay;  // 위에 조건이 맞으면 계속 해~
}

FString AYUGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		// 숫자랑 자리 모두 일치
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			// 숫자는 포함되지만 자리가 다를 때
			// GuessNumber을 문자열 형태로 바꿔서 가져오고
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))	// SecretNumber에 포함
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}


// GameMode는 서버 전용 액터니까 클라이언트에서 서버 RPC로 호출하기
void AYUGameModeBase::PrintChatMessageString(AYUPlayerController* InChattingPlayerController,
	const FString& InChatMessageString)
{
	int Index = InChatMessageString.Len() - 3;	// 문자열 끝에서 3글자 전의 위치르 Index에 저장
	FString GuessNumberString = InChatMessageString.RightChop(Index);	// 문자열에서 앞부분을 잘라내고 뒤쪽 나머지를 가져옴
	if (IsGuessNumberString(GuessNumberString) == true)	// 추측하려고 낸 번호인지 확인
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);	// 담기

		IncreaseGuessCount(InChattingPlayerController);	// 일반 채팅은 GuessCount 안 오르게 하기
		
		for (TActorIterator<AYUPlayerController> It(GetWorld()); It; ++It)	// 전체 플레이어 컨트롤러 순회 돌기
		{
			AYUPlayerController* YUPC = *It;
			if (IsValid(YUPC) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;  // 결과에 대한 문자열을 combine
				YUPC->ClientRPCPrintChatMessageString(CombinedMessageString);  // 클라한테

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));	// 좌측부터 잘라서 int로 바꿈 -> 제일 앞에 있는 숫자를 가져오면 StrikeCount 알 수 있음
				JudgeGame(InChattingPlayerController, StrikeCount);	// 시도한 애 누구야? 
			}
		}
	}
	else  // 추측 번호가 아니라 다른 걸 친 것 같아
	{
		for (TActorIterator<AYUPlayerController> It(GetWorld()); It; ++It)
		{
			AYUPlayerController* YUPlayerController = *It;
			if (IsValid(YUPlayerController) == true)
			{
				YUPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);	// 클라이언트한테  ChatMessage 그대로 넣어줌
			}
		}
	}
}

void AYUGameModeBase::IncreaseGuessCount(AYUPlayerController* InChattingPlayerController)
{
	AYUPlayerState* YUPS = InChattingPlayerController->GetPlayerState<AYUPlayerState>();	// PlayerState 가져와서
	if (IsValid(YUPS) == true)
	{
		YUPS->CurrentGuessCount++;	// CurrentGuessCount만 증가
	}
}

void AYUGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();	// 리셋 됐으니까 SecretNumber 새로 만들기

	for (const auto& PlayerController : AllPlayerControllers)	// 전체를 돌면서
	{
		AYUPlayerState* YUPS = PlayerController->GetPlayerState<AYUPlayerState>();
		if (IsValid(YUPS) == true)
		{
			YUPS->CurrentGuessCount = 0;
		}
	}
}

// 결과 판정하기
void AYUGameModeBase::JudgeGame(AYUPlayerController* InChattingPlayerController, int InStrikeCount)
{
	// 승리한 경우
	if (3 == InStrikeCount)	// 스트라이크가 3이면 정답
	{
		AYUPlayerState* YUPS = InChattingPlayerController->GetPlayerState<AYUPlayerState>();
		for (const auto& YUPlayerController : AllPlayerControllers)	// 접속한 모든 PC 돌면서
		{
			if (IsValid(YUPS) == true)
			{
				// PlayerNameString(Player1, 2, ..)을 문자열로 combine 한 후에
				FString CombinedMessageString = YUPS->PlayerNameString + TEXT(" has won the game.");	// 바인드 된 함수를 통해 변경됨 -> 너가 이김!
				// AllPlayerController 중 PlayerController 한 개의 NotificationText를 수정해줌
				YUPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	// 승리 안 한 경우 - 무승부, GuessCount 다 쓴 경우
	else
	{
		bool bIsDraw = true;
		for (const auto& YUPlayerController : AllPlayerControllers)	// 쭉 다 돌면서
		{
			AYUPlayerState*YUPS = YUPlayerController->GetPlayerState<AYUPlayerState>();
			if (IsValid(YUPS) == true)
			{
				if (YUPS->CurrentGuessCount < YUPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}

			if (true == bIsDraw)	// 무승부
			{
				for (const auto& UXPlayerController : AllPlayerControllers)
				{
					YUPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

					ResetGame();
				}
			}
		}
	}
}
