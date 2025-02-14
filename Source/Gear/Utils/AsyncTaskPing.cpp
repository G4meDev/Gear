// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/AsyncTaskPing.h"
#include "Http.h"

UAsyncTaskPing::UAsyncTaskPing(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UAsyncTaskPing* UAsyncTaskPing::PingUrl(FString URL, float Timeout)
{
	UAsyncTaskPing* PingTask = NewObject<UAsyncTaskPing>();
	PingTask->Start(URL, Timeout);

	return PingTask;
}

void UAsyncTaskPing::Start(FString URL, float Timeout)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL(URL);
	Request->SetTimeout(Timeout);
	RequestStartTime = FPlatformTime::Seconds();
	Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
		{
			RemoveFromRoot();

			double RequestEndTime = FPlatformTime::Seconds();
			double ResponseTime = RequestEndTime - RequestStartTime;
			if (bSuccess && Response.IsValid())
			{
				OnSuccess.Broadcast(ResponseTime);
			}
			else
			{
				OnTimeout.Broadcast(999.0f);
			}
		});

	Request->ProcessRequest();
}