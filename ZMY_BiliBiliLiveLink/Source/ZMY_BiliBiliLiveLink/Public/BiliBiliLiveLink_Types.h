#pragma once

#include "CoreMinimal.h"
#include "BiliBiliLiveLink_Types.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBiliBiliSimpleDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBiliBiliStringMessageDelegate, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBiliBiliRawMessageDelegate, const TArray<uint8>&, RawMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBiliBiliStringArrayMessageDelegate, const TArray<FString>&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBiliBiliWebSocketClosedDelegate, int32, StatusCode, const FString&, Reason, bool, bWasClean);

DECLARE_LOG_CATEGORY_EXTERN(LogBiliBili, Log, All);

UENUM(BlueprintType)
enum class EBiliBiliLiveLinkType_ValueType : uint8
{
	String,
	Integer,
	Float,
	Bool
};

USTRUCT(BlueprintType)
struct ZMY_BILIBILILIVELINK_API FBiliBiliLiveLinkType_KeyValuePair
{
	GENERATED_USTRUCT_BODY()

	FBiliBiliLiveLinkType_KeyValuePair() = default;

	FBiliBiliLiveLinkType_KeyValuePair(EBiliBiliLiveLinkType_ValueType _ValueType, const FString& _Key, const FString& _Value)
		: ValueType(_ValueType)
		, Key(_Key)
		, Value(_Value)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBiliBiliLiveLinkType_ValueType ValueType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;
};

USTRUCT(BlueprintType)
struct ZMY_BILIBILILIVELINK_API FBiliBiliLiveLinkType_WebSocketStartInfo
{
	GENERATED_USTRUCT_BODY()

	FBiliBiliLiveLinkType_WebSocketStartInfo() = default;

	// 场次ID，发送心跳时使用
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameID;

	// 鉴权信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AuthBody;

	// WebSocket地址
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WebSocketURL;

	// 主播房间号
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 RoomID = 0;

	// 主播昵称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UName;

	// 主播头像URL
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UFaceURL;

	// 主播UID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UID = 0;
};