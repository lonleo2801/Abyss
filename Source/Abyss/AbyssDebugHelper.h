// © 2025 Leon Lee. All rights reserved.

#pragma once


namespace AbyssDebug
{
	static void Print(const FString& Msg,int32 InKey = -1,const FColor& Color = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, Msg);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg)
		}
	}

	static void Print(const FString& FloatTitle,float FloatValueToPrint,int32 InKey =-1,const FColor& Color = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			const FString Msg = FloatTitle +TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);
			GEngine->AddOnScreenDebugMessage(InKey, 7.f, Color, Msg);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg)
		}
	}

	static void DrawArrow(UWorld* World,const FVector& Start,const FVector& Direction,float Duration = 0.0f,float Length = 200.0f,
		float ArrowSize = 60.0f,const FColor& Color = FColor::Red,float Thickness = 2.0f)
	{
		if (!World) return;

		FVector DirNormalized = Direction.GetSafeNormal();
		FVector End = Start + DirNormalized * Length;

		DrawDebugDirectionalArrow(
			World,
			Start,
			End,
			ArrowSize,
			Color,
			Duration > 0.0f, // true = 持久，false = 一帧
			Duration,
			0,
			Thickness
		);
	}
}