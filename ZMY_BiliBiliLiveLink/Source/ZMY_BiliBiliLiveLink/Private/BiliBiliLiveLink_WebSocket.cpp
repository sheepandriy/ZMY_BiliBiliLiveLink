// Fill out your copyright notice in the Description page of Project Settings.


#include "BiliBiliLiveLink_WebSocket.h"

UBiliBiliLiveLink_WebSocket* UBiliBiliLiveLink_WebSocket::CreateWebSocket(UObject* WorldContextObject, FString URL, FString Protocol, const TMap<FString, FString>& UpgradeHeaders)
{
	UBiliBiliLiveLink_WebSocket* OutWebSocket = NewObject<UBiliBiliLiveLink_WebSocket>(WorldContextObject);
	OutWebSocket->InitWebSocket(URL, Protocol, UpgradeHeaders);

	return OutWebSocket;
}

void UBiliBiliLiveLink_WebSocket::Connect()
{
	if (WebSocket.IsValid())
	{
		WebSocket->Connect();
	}
}

void UBiliBiliLiveLink_WebSocket::Close(int32 StatusCode, FString Reason)
{
	if (WebSocket.IsValid())
	{
		WebSocket->Close(StatusCode, Reason);
	}
}

void UBiliBiliLiveLink_WebSocket::SendString(FString DataString)
{
	if (WebSocket.IsValid())
	{
		WebSocket->Send(DataString);
	}
}

void UBiliBiliLiveLink_WebSocket::SendRaw(const TArray<uint8>& DataBytes, bool bIsBinary)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(DataBytes.GetData(), DataBytes.Num(), bIsBinary);
	}
}

bool UBiliBiliLiveLink_WebSocket::IsConnected()
{
	return WebSocket.IsValid() && WebSocket->IsConnected();
}

void UBiliBiliLiveLink_WebSocket::InitWebSocket(const FString& URL, const FString& Protocol, const TMap<FString, FString>& UpgradeHeaders)
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("WebSockets")) == false)
	{
		FModuleManager::Get().LoadModuleChecked(TEXT("WebSockets"));
	}

	WebSocket = FWebSocketsModule::Get().CreateWebSocket(URL, Protocol, UpgradeHeaders);
	WebSocket->OnConnected().AddUObject(this, &UBiliBiliLiveLink_WebSocket::HandleOnConnected);
	WebSocket->OnConnectionError().AddUObject(this, &UBiliBiliLiveLink_WebSocket::HandleOnConnectionError);
	WebSocket->OnMessage().AddUObject(this, &UBiliBiliLiveLink_WebSocket::HandleOnMessage);
	WebSocket->OnRawMessage().AddUObject(this, &UBiliBiliLiveLink_WebSocket::HandleOnRawMessage);
	WebSocket->OnClosed().AddUObject(this, &UBiliBiliLiveLink_WebSocket::HandleOnClosed);
}

void UBiliBiliLiveLink_WebSocket::HandleOnConnected()
{
	OnConnected.Broadcast();
}

void UBiliBiliLiveLink_WebSocket::HandleOnConnectionError(const FString& ErrorMessage)
{
	OnConnectionError.Broadcast(ErrorMessage);
}

void UBiliBiliLiveLink_WebSocket::HandleOnMessage(const FString& Message)
{
	OnReceivedMessage.Broadcast(Message);
}

void UBiliBiliLiveLink_WebSocket::HandleOnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
	SavedRawMessage.Append((const uint8*)Data, Size);

	if (BytesRemaining == 0)
	{
		OnReceivedRawMessage.Broadcast(SavedRawMessage);
		SavedRawMessage.Empty();
	}
}

void UBiliBiliLiveLink_WebSocket::HandleOnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	OnWebSocketClosed.Broadcast(StatusCode, Reason, bWasClean);
}
