// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "BiliBiliLiveLink_Types.h"
#include "BiliBiliLiveLink_HTTP.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZMY_BILIBILILIVELINK_API UBiliBiliLiveLink_HTTP : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable, Category = "BiliBiliLiveLink|HTTP")
	FBiliBiliStringMessageDelegate OnRequestSuccess;

	UPROPERTY(BlueprintAssignable, Category = "BiliBiliLiveLink|HTTP")
	FBiliBiliStringMessageDelegate OnRequestFail;

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|HTTP", meta = (WorldContext = "WorldContextObject"))
	static UBiliBiliLiveLink_HTTP* CreateHTTPRequest(UObject* WorldContextObject, FString Verb, FString URL, FString Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|HTTP")
	bool SendRequest();

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|HTTP")
	bool SendNewRequest(const FString& Verb, const FString& URL, const FString& Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header);

private:

	void InitHTTPRequest(const FString& Verb, const FString& URL, const FString& Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header);

	void SetHTTPParams(const FString& Verb, const FString& URL, const FString& Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header);

	void HandleRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = nullptr;
};
