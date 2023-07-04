// Fill out your copyright notice in the Description page of Project Settings.


#include "BiliBiliLiveLink_HTTP.h"

UBiliBiliLiveLink_HTTP* UBiliBiliLiveLink_HTTP::CreateHTTPRequest(UObject* WorldContextObject, FString Verb, FString URL, FString Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header)
{
	UBiliBiliLiveLink_HTTP* HTTP = NewObject<UBiliBiliLiveLink_HTTP>(WorldContextObject);
	HTTP->InitHTTPRequest(Verb, URL, Content, Header);
	return HTTP;
}

bool UBiliBiliLiveLink_HTTP::SendRequest()
{
	return HttpRequest.IsValid() ? HttpRequest->ProcessRequest() : false;
}

bool UBiliBiliLiveLink_HTTP::SendNewRequest(const FString& Verb, const FString& URL, const FString& Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header)
{
	SetHTTPParams(Verb, URL, Content, Header);
	return HttpRequest.IsValid() ? HttpRequest->ProcessRequest() : false;
}

void UBiliBiliLiveLink_HTTP::InitHTTPRequest(const FString& Verb, const FString& URL, const FString& Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header)
{
	HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UBiliBiliLiveLink_HTTP::HandleRequestComplete);
	SetHTTPParams(Verb, URL, Content, Header);
}

void UBiliBiliLiveLink_HTTP::SetHTTPParams(const FString& Verb, const FString& URL, const FString& Content, const TArray<FBiliBiliLiveLinkType_KeyValuePair>& Header)
{
	if (HttpRequest.IsValid() == false)
	{
		return;
	}

	HttpRequest->SetVerb(Verb);
	HttpRequest->SetURL(URL);

	if (Content.IsEmpty() == false)
	{
		HttpRequest->SetContentAsString(Content);
	}

	for (const auto& Element : Header)
	{
		HttpRequest->SetHeader(Element.Key, Element.Value);
	}
}

void UBiliBiliLiveLink_HTTP::HandleRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FString Message = Response.IsValid() ? Response->GetContentAsString() : TEXT("Unknown");

	if (bConnectedSuccessfully == false)
	{
		OnRequestFail.Broadcast(Message);
		return;
	}

	OnRequestSuccess.Broadcast(Message);
}
