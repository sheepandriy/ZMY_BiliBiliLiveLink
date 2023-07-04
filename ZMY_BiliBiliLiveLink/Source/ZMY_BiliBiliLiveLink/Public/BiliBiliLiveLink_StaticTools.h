// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BiliBiliLiveLink_Types.h"
#include "BiliBiliLiveLink_StaticTools.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ZMY_BILIBILILIVELINK_API UBiliBiliLiveLink_StaticTools : public UObject
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|StaticTools")
	static TArray<FBiliBiliLiveLinkType_KeyValuePair> MakeHeader(FString AccessKeyId, FString AccessSecret, FString Params);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|StaticTools")
	static FString MakeSign(const TArray<FBiliBiliLiveLinkType_KeyValuePair>& SignInfo, FString Secret);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|StaticTools")
	static FString PairsToString(const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Pairs);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|StaticTools")
	static FString HMACString(const FString& Info, const FString& Secret);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|StaticTools")
	static FString ConvertPairsToJsonString(const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Pairs);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|StaticTools")
	static TArray<uint8> PackMessage(int32 OperationCode, const FString& MessageBody);

	UFUNCTION(BlueprintCallable, Category = "BiliBiliLiveLink|StaticTools")
	static TArray<FString> UnpackMessage(const TArray<uint8>& RawMessage, int32& OperationCode);

	template<class T>
	static TArray<T> ArraySlice(const TArray<T>& InArray, int32 From, int32 To)
	{
		TArray<T> OutArray;
		if (InArray.Num() >= To)
		{
			for (int32 Index = From; Index < To; Index++)
			{
				OutArray.Add(InArray[Index]);
			}
		}
		
		return OutArray;
	}

	static const int16 BiliBiliWebSocketHeaderLength;
	static const int16 BiliBiliWebSocketVersion;
	static const int32 BiliBiliWebSocketSequenceID;
};
