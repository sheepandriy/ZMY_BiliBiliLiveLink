// Fill out your copyright notice in the Description page of Project Settings.


#include "BiliBiliLiveLinkSubsystem.h"
#include "BiliBiliLiveLink_StaticTools.h"
#include "BiliBiliLiveLink_HTTP.h"
#include "BiliBiliLiveLink_WebSocket.h"
#include "Json.h"
#include "Dom/JsonObject.h"

FString UBiliBiliLiveLinkSubsystem::HTTPUrl_Start = TEXT("https://live-open.biliapi.com/v2/app/start");
FString UBiliBiliLiveLinkSubsystem::HTTPUrl_End = TEXT("https://live-open.biliapi.com/v2/app/end");
FString UBiliBiliLiveLinkSubsystem::HTTPUrl_Heartbeat = TEXT("https://live-open.biliapi.com/v2/app/heartbeat");
FString UBiliBiliLiveLinkSubsystem::HTTPUrl_BatchHeartbeat = TEXT("https://live-open.biliapi.com/v2/app/batchHeartbeat");

UBiliBiliLiveLinkSubsystem* UBiliBiliLiveLinkSubsystem::Get(UObject* WorldContextObject)
{
	if (IsValid(GEngine))
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return UGameInstance::GetSubsystem<UBiliBiliLiveLinkSubsystem>(World->GetGameInstance());
		}
	}
	return nullptr;
}

void UBiliBiliLiveLinkSubsystem::InitializeAccessInfo(const FString& InAccessKeyID, const FString& InAccessSecret)
{
	AccessKeyID = InAccessKeyID;
	AccessSecret = InAccessSecret;
}

void UBiliBiliLiveLinkSubsystem::Start(FString InIdentityCode, FString InAppID)
{
	IdentityCode = InIdentityCode;
	AppID = InAppID;
	SendStartRequest();
}

void UBiliBiliLiveLinkSubsystem::End()
{
	if (WebSocket && WebSocket->IsConnected())
	{
		FString Params = FString::Printf(TEXT("{\"app_id\":%I64d},\"game_id\":\"%s\""), FCString::Atoi64(*AppID), *WebSocketStartInfo.GameID);
		TArray<FBiliBiliLiveLinkType_KeyValuePair> Header = UBiliBiliLiveLink_StaticTools::MakeHeader(AccessKeyID, AccessSecret, Params);

		EndHTTPRequest = UBiliBiliLiveLink_HTTP::CreateHTTPRequest(this, TEXT("POST"), HTTPUrl_End, Params, Header);
		EndHTTPRequest->SendRequest();

		WebSocket->Close();
	}

	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.ClearTimer(AppHeartTimerHandle);
		TimerManager.ClearTimer(LongHeartTimerHandle);
	}
}

const FString& UBiliBiliLiveLinkSubsystem::GetAccessKeyID() const
{
	return AccessKeyID;
}

const FString& UBiliBiliLiveLinkSubsystem::GetAccessSecret() const
{
	return AccessSecret;
}

const FString& UBiliBiliLiveLinkSubsystem::GetIdentityCode() const
{
	return IdentityCode;
}

const FString& UBiliBiliLiveLinkSubsystem::GetAppID() const
{
	return AppID;
}

void UBiliBiliLiveLinkSubsystem::ModifyHTTPUrl_Start(const FString& NewUrl)
{
	HTTPUrl_Start = NewUrl;
}

void UBiliBiliLiveLinkSubsystem::ModifyHTTPUrl_End(const FString& NewUrl)
{
	HTTPUrl_End = NewUrl;
}

void UBiliBiliLiveLinkSubsystem::ModifyHTTPUrl_Heartbeat(const FString& NewUrl)
{
	HTTPUrl_Heartbeat = NewUrl;
}

void UBiliBiliLiveLinkSubsystem::ModifyHTTPUrl_BatchHeartbeat(const FString& NewUrl)
{
	HTTPUrl_BatchHeartbeat = NewUrl;
}

void UBiliBiliLiveLinkSubsystem::SendStartRequest()
{
	FString Params = FString::Printf(TEXT("{\"code\":\"%s\",\"app_id\":%I64d}"), *IdentityCode, FCString::Atoi64(*AppID));
	TArray<FBiliBiliLiveLinkType_KeyValuePair> Header = UBiliBiliLiveLink_StaticTools::MakeHeader(AccessKeyID, AccessSecret, Params);

	StartHTTPReqeust = UBiliBiliLiveLink_HTTP::CreateHTTPRequest(this, TEXT("POST"), HTTPUrl_Start, Params, Header);
	StartHTTPReqeust->OnRequestFail.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleStartRequestFail);
	StartHTTPReqeust->OnRequestSuccess.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleStartRequestSuccess);
	StartHTTPReqeust->SendRequest();
}

void UBiliBiliLiveLinkSubsystem::HandleStartRequestFail(const FString& FailMessage)
{
	OnStartRequestFail.Broadcast(FailMessage);
}

void UBiliBiliLiveLinkSubsystem::HandleStartRequestSuccess(const FString& Message)
{
	TSharedPtr<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<>::Create(Message);
	if (FJsonSerializer::Deserialize(Reader, JsonObj))
	{
		int64 Code = INDEX_NONE;
		if (JsonObj->TryGetNumberField(TEXT("code"), Code) && Code == 0)
		{
			StartJsonString = Message;
			if (TryReadStartJson())
			{
				Connect();
				return;
			}
		}
	}

	StartRequestFail(Message);
}

void UBiliBiliLiveLinkSubsystem::StartRequestFail(const FString& Message)
{
	OnStartRequestFail.Broadcast(Message);
}

void UBiliBiliLiveLinkSubsystem::Connect()
{
	WebSocket = UBiliBiliLiveLink_WebSocket::CreateWebSocket(this, WebSocketStartInfo.WebSocketURL, FString(), TMap<FString, FString>());
	WebSocket->OnConnected.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleConnected);
	WebSocket->OnConnectionError.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleConnectionError);
	WebSocket->OnReceivedRawMessage.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleReceivedRawMessage);
	WebSocket->OnWebSocketClosed.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleWebSocketClosed);
	WebSocket->Connect();
}

bool UBiliBiliLiveLinkSubsystem::TryReadStartJson()
{
	TSharedPtr<FJsonObject> WebSocketInfoJsonObject = MakeShared<FJsonObject>();

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<>::Create(StartJsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, WebSocketInfoJsonObject))
	{
		StartRequestFail(StartJsonString);
		return false;
	}

	const TSharedPtr<FJsonObject>* Data = nullptr;
	if (!WebSocketInfoJsonObject->TryGetObjectField(TEXT("data"), Data))
	{
		StartRequestFail(StartJsonString);
		return false;
	}

	WebSocketStartData = *Data;

	return GetGameInfo() && GetWebSocketInfo() && GetAnchorInfo();
}

bool UBiliBiliLiveLinkSubsystem::GetGameInfo()
{
	const TSharedPtr<FJsonObject>* GameInfo = nullptr;
	if (!WebSocketStartData->TryGetObjectField(TEXT("game_info"), GameInfo))
	{
		StartRequestFail(TEXT("game_info"));
		return false;
	}

	if (!(*GameInfo)->TryGetStringField(TEXT("game_id"), WebSocketStartInfo.GameID))
	{
		StartRequestFail(TEXT("game_id"));
		return false;
	}

	return true;
}

bool UBiliBiliLiveLinkSubsystem::GetWebSocketInfo()
{
	const TSharedPtr<FJsonObject>* WebSocketInfo = nullptr;
	if (!WebSocketStartData->TryGetObjectField(TEXT("websocket_info"), WebSocketInfo))
	{
		StartRequestFail(TEXT("websocket_info"));
		return false;
	}

	TArray<FString> UrlArray;
	if (!(*WebSocketInfo)->TryGetStringArrayField(TEXT("wss_link"), UrlArray) || UrlArray.Num() <= 0)
	{
		StartRequestFail(TEXT("wss_link"));
		return false;
	}

	WebSocketStartInfo.WebSocketURL = UrlArray[0];

	if (!(*WebSocketInfo)->TryGetStringField(TEXT("auth_body"), WebSocketStartInfo.AuthBody))
	{
		StartRequestFail(TEXT("auth_body"));
		return false;
	}

	return true;
}

bool UBiliBiliLiveLinkSubsystem::GetAnchorInfo()
{
	const TSharedPtr<FJsonObject>* AnchorInfo = nullptr;
	if (!WebSocketStartData->TryGetObjectField(TEXT("anchor_info"), AnchorInfo))
	{
		StartRequestFail(TEXT("anchor_info"));
		return false;
	}

	if (!(*AnchorInfo)->TryGetNumberField(TEXT("room_id"), WebSocketStartInfo.RoomID))
	{
		StartRequestFail(TEXT("room_id"));
		return false;
	}

	if (!(*AnchorInfo)->TryGetStringField(TEXT("uname"), WebSocketStartInfo.UName))
	{
		StartRequestFail(TEXT("uname"));
		return false;
	}

	if (!(*AnchorInfo)->TryGetStringField(TEXT("uface"), WebSocketStartInfo.UFaceURL))
	{
		StartRequestFail(TEXT("uface"));
		return false;
	}

	if (!(*AnchorInfo)->TryGetNumberField(TEXT("uid"), WebSocketStartInfo.UID))
	{
		StartRequestFail(TEXT("uid"));
		return false;
	}

	return true;
}

void UBiliBiliLiveLinkSubsystem::HandleConnected()
{
	if (WebSocket)
	{
		WebSocket->SendRaw(UBiliBiliLiveLink_StaticTools::PackMessage(7, WebSocketStartInfo.AuthBody), true);
	}
}

void UBiliBiliLiveLinkSubsystem::HandleConnectionError(const FString& ErrorMessage)
{
	OnStartRequestFail.Broadcast(ErrorMessage);
}

void UBiliBiliLiveLinkSubsystem::HandleReceivedRawMessage(const TArray<uint8>& RawMessage)
{
	int32 Operation = 0;
	WebsocketReceivedString = UBiliBiliLiveLink_StaticTools::UnpackMessage(RawMessage, Operation);

	if (Operation == 8)
	{
		HandleReceiveAuthReply();
	}
	else if (Operation == 3 && WebsocketReceivedString.Num() > 0)
	{
		UE_LOG(LogBiliBili, Warning, TEXT("Long heart res:%s"), *WebsocketReceivedString[0])
	}
	else if (Operation == 5 && WebsocketReceivedString.Num() > 0)
	{
		OnReceivedDanMuJson.Broadcast(WebsocketReceivedString);
	}
}

void UBiliBiliLiveLinkSubsystem::HandleWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	OnWebSocketClosed.Broadcast(StatusCode, Reason, bWasClean);
}

void UBiliBiliLiveLinkSubsystem::HandleReceiveAuthReply()
{
	if (WebsocketReceivedString.Num() <= 0)
	{
		return;
	}

	TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<>::Create(WebsocketReceivedString[0]);
	if (FJsonSerializer::Deserialize(Reader, RootObject))
	{
		int32 ReplyCode = INDEX_NONE;
		ReplyCode = RootObject->GetIntegerField(TEXT("code"));

		if (ReplyCode == 0)
		{
			OnConnected.Broadcast();
			MakeHeart();
		}
		else
		{
			OnStartRequestFail.Broadcast(TEXT("{\"message\":\"验证失败！\"}"));
			if (WebSocket)
			{
				WebSocket->Close(INDEX_NONE, TEXT("{\"message\":\"验证失败！\"}"));
			}
		}
	}
}

void UBiliBiliLiveLinkSubsystem::MakeHeart()
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(AppHeartTimerHandle, this, &UBiliBiliLiveLinkSubsystem::AppHeart, 20.0f, true);
		TimerManager.SetTimer(LongHeartTimerHandle, this, &UBiliBiliLiveLinkSubsystem::LongHeart, 30.0f, true);
	}
}

void UBiliBiliLiveLinkSubsystem::AppHeart()
{
	FString Params = FString::Printf(TEXT("{\"game_id\":\"%s\"}"), *WebSocketStartInfo.GameID);
	TArray<FBiliBiliLiveLinkType_KeyValuePair> Header = UBiliBiliLiveLink_StaticTools::MakeHeader(AccessKeyID, AccessSecret, Params);

	HeartHTTPRequest = UBiliBiliLiveLink_HTTP::CreateHTTPRequest(this, TEXT("POST"), HTTPUrl_Heartbeat, Params, Header);
	HeartHTTPRequest->OnRequestSuccess.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleReceivedAppHeartRes);
	HeartHTTPRequest->OnRequestFail.AddDynamic(this, &UBiliBiliLiveLinkSubsystem::HandleReceivedAppHeartRes);
	HeartHTTPRequest->SendRequest();
}

void UBiliBiliLiveLinkSubsystem::LongHeart()
{
	if (WebSocket)
	{
		WebSocket->SendRaw(UBiliBiliLiveLink_StaticTools::PackMessage(2, FString()), true);
	}
}

void UBiliBiliLiveLinkSubsystem::HandleReceivedAppHeartRes(const FString& Message)
{
	UE_LOG(LogBiliBili, Warning, TEXT("AppHeartRes:%s"), *Message);
}
