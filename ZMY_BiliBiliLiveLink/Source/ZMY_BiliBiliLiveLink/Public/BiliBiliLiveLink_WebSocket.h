// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "BiliBiliLiveLink_Types.h"
#include "BiliBiliLiveLink_WebSocket.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZMY_BILIBILILIVELINK_API UBiliBiliLiveLink_WebSocket : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "BiliBiliLiveLink|WebSockets")
	FBiliBiliSimpleDelegate OnConnected;

	UPROPERTY(BlueprintAssignable, Category = "BiliBiliLiveLink|WebSockets")
	FBiliBiliStringMessageDelegate OnConnectionError;

	UPROPERTY(BlueprintAssignable, Category = "BiliBiliLiveLink|WebSockets")
	FBiliBiliStringMessageDelegate OnReceivedMessage;

	UPROPERTY(BlueprintAssignable, Category = "BiliBiliLiveLink|WebSockets")
	FBiliBiliRawMessageDelegate OnReceivedRawMessage;

	UPROPERTY(BlueprintAssignable, Category = "BiliBiliLiveLink|WebSockets")
	FBiliBiliWebSocketClosedDelegate OnWebSocketClosed;

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|WebSockets", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm = "UpgradeHeaders"))
	static UBiliBiliLiveLink_WebSocket* CreateWebSocket(UObject* WorldContextObject, FString URL, FString Protocol, const TMap<FString, FString>& UpgradeHeaders);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|WebSockets")
	void Connect();

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|WebSockets")
	void Close(int32 StatusCode = 1000, FString Reason = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|WebSockets")
	void SendString(FString DataString);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|WebSockets")
	void SendRaw(const TArray<uint8>& DataBytes, bool bIsBinary = false);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|WebSockets")
	bool IsConnected();
	
private:

	void InitWebSocket(const FString& URL, const FString& Protocol, const TMap<FString, FString>& UpgradeHeaders);

	void HandleOnConnected();

	void HandleOnConnectionError(const FString& ErrorMessage);

	void HandleOnMessage(const FString& Message);

	void HandleOnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining);

	void HandleOnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);

	TSharedPtr<IWebSocket> WebSocket = nullptr;

	TArray<uint8> SavedRawMessage;
};
