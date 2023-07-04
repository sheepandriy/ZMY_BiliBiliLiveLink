// Fill out your copyright notice in the Description page of Project Settings.


#include "BiliBiliLiveLink_StaticTools.h"
#include "Json.h"
#include "Dom/JsonObject.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <zlib.h>
THIRD_PARTY_INCLUDES_END
#undef UI

const int16 UBiliBiliLiveLink_StaticTools::BiliBiliWebSocketHeaderLength = 16;
const int16 UBiliBiliLiveLink_StaticTools::BiliBiliWebSocketVersion = 0;
const int32 UBiliBiliLiveLink_StaticTools::BiliBiliWebSocketSequenceID = 0;

TArray<FBiliBiliLiveLinkType_KeyValuePair> UBiliBiliLiveLink_StaticTools::MakeHeader(FString AccessKeyId, FString AccessSecret, FString Params)
{
	FString Timestamp = FString::Printf(TEXT("%I64d"), FDateTime::Now().ToUnixTimestamp());
	FString Nonce = FString::FromInt(FMath::Rand()) + Timestamp;

	TArray<FBiliBiliLiveLinkType_KeyValuePair> OutHeader =
	{
		{EBiliBiliLiveLinkType_ValueType::String, TEXT("x-bili-accesskeyid"), AccessKeyId},
		{EBiliBiliLiveLinkType_ValueType::String, TEXT("x-bili-content-md5"), FMD5::HashAnsiString(*Params)},
		{EBiliBiliLiveLinkType_ValueType::String, TEXT("x-bili-signature-method"), TEXT("HMAC-SHA256")},
		{EBiliBiliLiveLinkType_ValueType::String, TEXT("x-bili-signature-nonce"), Nonce},
		{EBiliBiliLiveLinkType_ValueType::String, TEXT("x-bili-signature-version"), TEXT("1.0")},
		{EBiliBiliLiveLinkType_ValueType::String, TEXT("x-bili-timestamp"), Timestamp}
	};

	FString SignString = MakeSign(OutHeader, AccessSecret);

	OutHeader.Append
	(
		{
			{EBiliBiliLiveLinkType_ValueType::String, TEXT("Authorization"), SignString},
			{EBiliBiliLiveLinkType_ValueType::String, TEXT("Content-Type"), TEXT("application/json")},
			{EBiliBiliLiveLinkType_ValueType::String, TEXT("Accept"), TEXT("application/json")}
		}
	);

	return OutHeader;
}

FString UBiliBiliLiveLink_StaticTools::MakeSign(const TArray<FBiliBiliLiveLinkType_KeyValuePair>& SignInfo, FString Secret)
{
	FString SignString = PairsToString(SignInfo);
	return HMACString(SignString, Secret);
}

FString UBiliBiliLiveLink_StaticTools::PairsToString(const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Pairs)
{
	FString OutString;
	for (const auto& Element : Pairs)
	{
		FString TempString = Element.Key + TEXT(":") + Element.Value + TEXT("\n");
		OutString.Append(TempString);
	}

	OutString.RemoveFromEnd(TEXT("\n"));

	return OutString;
}

FString UBiliBiliLiveLink_StaticTools::HMACString(const FString& Info, const FString& Secret)
{
	FTCHARToUTF8 InfoBytesConvert(*Info);
	FTCHARToUTF8 SecretBytesConvert(*Secret);

	uint8 SHA256Code[32];
	unsigned int CodeLength = 0;

	HMAC(EVP_sha256(), SecretBytesConvert.Get(), SecretBytesConvert.Length(), (const unsigned char*)InfoBytesConvert.Get(), InfoBytesConvert.Length(), SHA256Code, &CodeLength);

	FString OutString;
	for (int32 i = 0; i < 32; i++)
	{
		OutString.Append(FString::Printf(TEXT("%02x"), SHA256Code[i]));
	}

	return OutString;
}

FString UBiliBiliLiveLink_StaticTools::ConvertPairsToJsonString(const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Pairs)
{
	FString OutJsonString;

	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutJsonString);

	JsonWriter->WriteObjectStart();
	for (const auto& Element : Pairs)
	{
		switch (Element.ValueType)
		{
		case EBiliBiliLiveLinkType_ValueType::String:
			JsonWriter->WriteValue(Element.Key, Element.Value);
			break;
		case EBiliBiliLiveLinkType_ValueType::Integer:
			JsonWriter->WriteValue(Element.Key, FCString::Atoi64(*Element.Value));
			break;
		case EBiliBiliLiveLinkType_ValueType::Float:
			JsonWriter->WriteValue(Element.Key, FCString::Atod(*Element.Value));
			break;
		case EBiliBiliLiveLinkType_ValueType::Bool:
			JsonWriter->WriteValue(Element.Key, Element.Value.ToBool());
			break;
		}
	}
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	return OutJsonString;
}

TArray<uint8> UBiliBiliLiveLink_StaticTools::PackMessage(int32 OperationCode, const FString& MessageBody)
{
	FTCHARToUTF8 BodyConvert(*MessageBody);

	int32 BodyLength = BodyConvert.Length();
	int32 PackLengthSwapped = ByteSwap(BodyLength + BiliBiliWebSocketHeaderLength);
	int16 HeaderLengthSwapped = ByteSwap(BiliBiliWebSocketHeaderLength);
	int16 VersionSwapped = ByteSwap(BiliBiliWebSocketVersion);
	int32 OperationCodeSwapped = ByteSwap(OperationCode);
	int32 SequenceIdSwapped = ByteSwap(BiliBiliWebSocketSequenceID);

	TArray<uint8> OutBytes;
	OutBytes.Append((uint8*)&PackLengthSwapped, 4);
	OutBytes.Append((uint8*)&HeaderLengthSwapped, 2);
	OutBytes.Append((uint8*)&VersionSwapped, 2);
	OutBytes.Append((uint8*)&OperationCodeSwapped, 4);
	OutBytes.Append((uint8*)&SequenceIdSwapped, 4);
	OutBytes.Append((uint8*)BodyConvert.Get(), BodyLength);

	return OutBytes;
}

TArray<FString> UBiliBiliLiveLink_StaticTools::UnpackMessage(const TArray<uint8>& RawMessage, int32& OutOperationCode)
{
	TArray<FString> OutString;
	if (RawMessage.Num() > 16)
	{
		int32 PackLength = 0;
		int16 HeaderLength = 0;
		int16 Version = 0;
		int32 Operation = 0;
		int32 SequenceID = 0;

		FMemory::Memcpy(&PackLength, ArraySlice(RawMessage, 0, 4).GetData(), 4);
		FMemory::Memcpy(&HeaderLength, ArraySlice(RawMessage, 4, 6).GetData(), 2);
		FMemory::Memcpy(&Version, ArraySlice(RawMessage, 6, 8).GetData(), 2);
		FMemory::Memcpy(&Operation, ArraySlice(RawMessage, 8, 12).GetData(), 4);
		FMemory::Memcpy(&SequenceID, ArraySlice(RawMessage, 12, 16).GetData(), 4);

		PackLength = ByteSwap(PackLength);
		HeaderLength = ByteSwap(HeaderLength);
		Version = ByteSwap(Version);
		OutOperationCode = ByteSwap(Operation);
		SequenceID = ByteSwap(SequenceID);

		if (PackLength > 16)
		{
			TArray<uint8> MessageBodyRaw = ArraySlice(RawMessage, 16, PackLength);

			if (Version == 0)
			{
				OutString.Emplace(FUTF8ToTCHAR((ANSICHAR*)MessageBodyRaw.GetData(), MessageBodyRaw.Num()));
			}
			else if (Version == 2)
			{
				TArray<uint8> UncompressBuffer;
				uLong UncompressBufferLength = 100000;
				UncompressBuffer.SetNum(UncompressBufferLength);

				if (uncompress(UncompressBuffer.GetData(), &UncompressBufferLength, MessageBodyRaw.GetData(), MessageBodyRaw.Num()) == 0)
				{
					uint32 Offset = 0;
					while (Offset < UncompressBufferLength)
					{
						int32 SubPacketLength = 0;
						FMemory::Memcpy(&SubPacketLength, ArraySlice(UncompressBuffer, Offset, Offset + 4).GetData(), 4);
						SubPacketLength = ByteSwap(SubPacketLength);

						TArray<uint8> SubMessageBodyRaw = ArraySlice(UncompressBuffer, Offset + 16, SubPacketLength);
						OutString.Emplace(FUTF8ToTCHAR((ANSICHAR*)SubMessageBodyRaw.GetData(), SubMessageBodyRaw.Num()));

						Offset += SubPacketLength;
					}
				}
			}
			else
			{
				OutString.Emplace(TEXT("Version error"));
			}
		}
		else
		{
			OutString.Emplace(TEXT("PackLength error"));
		}
	}

	return OutString;
}
