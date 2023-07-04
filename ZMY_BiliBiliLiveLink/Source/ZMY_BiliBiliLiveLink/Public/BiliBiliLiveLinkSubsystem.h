// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BiliBiliLiveLink_Types.h"
#include "BiliBiliLiveLinkSubsystem.generated.h"

class UBiliBiliLiveLink_HTTP;
class UBiliBiliLiveLink_WebSocket;
class FJsonObject;

/**
 * 
 */
UCLASS(BlueprintType)
class ZMY_BILIBILILIVELINK_API UBiliBiliLiveLinkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static UBiliBiliLiveLinkSubsystem* Get(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|Subsystem")
	void InitializeAccessInfo(const FString& InAccessKeyID, const FString& InAccessSecret);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|Subsystem")
	void Start(FString InIdentityCode, FString InAppID);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|Subsystem")
	void End();

	const FString& GetAccessKeyID() const;
	const FString& GetAccessSecret() const;
	const FString& GetIdentityCode() const;
	const FString& GetAppID() const;

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|HTTPUrl")
	static void ModifyHTTPUrl_Start(const FString& NewUrl);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|HTTPUrl")
	static void ModifyHTTPUrl_End(const FString& NewUrl);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|HTTPUrl")
	static void ModifyHTTPUrl_Heartbeat(const FString& NewUrl);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|HTTPUrl")
	static void ModifyHTTPUrl_BatchHeartbeat(const FString& NewUrl);

private:

	static FString HTTPUrl_Start;
	static FString HTTPUrl_End;
	static FString HTTPUrl_Heartbeat;
	static FString HTTPUrl_BatchHeartbeat;

	void SendStartRequest();

	UFUNCTION()
	void HandleStartRequestFail(const FString& Message);

	UFUNCTION()
	void HandleStartRequestSuccess(const FString& Message);

	void StartRequestFail(const FString& Message);
	void Connect();

	bool TryReadStartJson();
	bool GetGameInfo();
	bool GetWebSocketInfo();
	bool GetAnchorInfo();

	UFUNCTION()
	void HandleConnected();

	UFUNCTION()
	void HandleConnectionError(const FString& ErrorMessage);

	UFUNCTION()
	void HandleReceivedRawMessage(const TArray<uint8>& RawMessage);

	UFUNCTION()
	void HandleWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);

	void HandleReceiveAuthReply();

	void MakeHeart();
	void AppHeart();
	void LongHeart();

	UFUNCTION()
	void HandleReceivedAppHeartRes(const FString& Message);

	FString StartJsonString;
	TArray<FString> WebsocketReceivedString;

	TSharedPtr<FJsonObject> WebSocketStartData = nullptr;
	FBiliBiliLiveLinkType_WebSocketStartInfo WebSocketStartInfo;

	UPROPERTY(BlueprintReadOnly, Getter, meta = (AllowPrivateAccess = true))
	FString AccessKeyID;

	UPROPERTY(BlueprintReadOnly, Getter, meta = (AllowPrivateAccess = true))
	FString AccessSecret;

	UPROPERTY(BlueprintReadOnly, Getter, meta = (AllowPrivateAccess = true))
	FString IdentityCode;

	UPROPERTY(BlueprintReadOnly, Getter, meta = (AllowPrivateAccess = true))
	FString AppID;

	UPROPERTY()
	TObjectPtr<UBiliBiliLiveLink_HTTP> StartHTTPReqeust = nullptr;

	UPROPERTY()
	TObjectPtr<UBiliBiliLiveLink_HTTP> HeartHTTPRequest = nullptr;

	UPROPERTY()
	TObjectPtr<UBiliBiliLiveLink_HTTP> EndHTTPRequest = nullptr;

	UPROPERTY()
	TObjectPtr<UBiliBiliLiveLink_WebSocket> WebSocket = nullptr;

	FTimerHandle AppHeartTimerHandle;

	FTimerHandle LongHeartTimerHandle;

public:

	UPROPERTY(BlueprintAssignable)
	FBiliBiliStringMessageDelegate OnStartRequestFail;

	UPROPERTY(BlueprintAssignable)
	FBiliBiliStringArrayMessageDelegate OnReceivedDanMuJson;

	UPROPERTY(BlueprintAssignable)
	FBiliBiliSimpleDelegate OnConnected;

	UPROPERTY(BlueprintAssignable)
	FBiliBiliWebSocketClosedDelegate OnWebSocketClosed;
};
