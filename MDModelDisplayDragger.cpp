void UMDModelDisplayDragger::OnFixedTimerTick()
{
	/*if (SequenceImages.Num() == 0 || DisplayVideo == nullptr)
	{
		StopSequence();
		return;
	}*/
	if (DisplayVideo == nullptr)
	{
		return;
	}
	
	// 每次固定频率定时器到达时，累积时间增加
	float FixedInterval = 1.0f / 60.0f;
	AccumulatedTime += FixedInterval;

	float DesiredInterval = 1.0f / FMath::Abs(FrameRate);

	// 当累积时间达到所需间隔时才更新帧
	while (AccumulatedTime >= DesiredInterval)
	{
		AccumulatedTime -= DesiredInterval; // 减去一帧间隔

		// 根据符号决定正向或反向播放
		if (FrameRate > 0.f)
		{
			CurrentFrameIndex++;
			//if (CurrentFrameIndex >= SequenceImages.Num())
			if (CurrentFrameIndex >= SequencePaths.Num())
			{
				CurrentFrameIndex = 0;
			}
		}
		else
		{
			CurrentFrameIndex--;
			if (CurrentFrameIndex < 0)
			{
				//CurrentFrameIndex = SequenceImages.Num() - 1;
				CurrentFrameIndex = SequencePaths.Num() - 1;
			}
		}
		
		//UE_LOG(LogTemp, Warning, TEXT("尝试播放: CurrentFrameIndex: %d, FrameRate: %.1f, AccumulatedTime: %.3f"), CurrentFrameIndex, FrameRate, AccumulatedTime)
		// 更新显示
		//if (SequenceImages.IsValidIndex(CurrentFrameIndex))

		//Check if data of SequenceImages_Dynamic on current frame is valid 
		if (SequenceImages_Dynamic.IsValidIndex(CurrentFrameIndex) && SequenceImages_Dynamic[CurrentFrameIndex] == nullptr)
		{
			//Check if current image path is valid
			if (SequencePaths.IsValidIndex(CurrentFrameIndex))
			{
				FTransformImageParams TransformSequenceParams;
				UTexture2D* LoadedSequence;
				bool bSuccess = false;
				FString Error;
				//Loading image for current frame
				if (URuntimeImageLoader* RuntimeImageLoader = Overlay->GetWorld()->GetSubsystem<URuntimeImageLoader>())
				{
					RuntimeImageLoader->LoadImageSync(SequencePaths[CurrentFrameIndex], TransformSequenceParams, LoadedSequence, bSuccess, Error);
					SequenceImages_Dynamic[CurrentFrameIndex] = LoadedSequence;
					if (bSuccess)
					{
						DisplayVideo->SetBrushFromTexture(SequenceImages_Dynamic[CurrentFrameIndex]);
						UE_LOG(LogTemp, Warning, TEXT("正在播放: %s"), *SequencePaths[CurrentFrameIndex])
					}
					else
					{
						UE_LOG(LogTemp, Display, TEXT("Load image failed! Path: %s"), *SequencePaths[CurrentFrameIndex])
					}
					
					//After displaying current sequence image, check if the follow-up images loading is needed
					//Counts of images need to be loaded upon the next check
					const int8 CountUponCheckup = 20;
					for (int32 i = 0; i < CountUponCheckup; i++)
					{
						if (SequenceImages_Dynamic[CurrentFrameIndex + i] != nullptr)
						{
							break;
						}
						if (!SequencePaths[CurrentFrameIndex + i].IsEmpty())
						{
							RuntimeImageLoader->LoadImageSync(SequencePaths[CurrentFrameIndex + i], TransformSequenceParams, LoadedSequence, bSuccess, Error);
							UE_LOG(LogTemp, Display, TEXT("Loading image upon for future index: %d"), CurrentFrameIndex + i)
						}
						SequenceImages_Dynamic[CurrentFrameIndex + i] = LoadedSequence;
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Invalid sequence path: %d"), CurrentFrameIndex)
			}
		}
		else
		{
			DisplayVideo->SetBrushFromTexture(SequenceImages_Dynamic[CurrentFrameIndex]);
			UE_LOG(LogTemp, Display, TEXT("Displaying image from SequenceImages_Dynamic"))
		}
	}
}
