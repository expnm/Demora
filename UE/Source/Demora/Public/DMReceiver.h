// exp.dev free

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"

#include "DMReceiver.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiverUpdated, const FString&, Message);

UCLASS()
class DEMORA_API ADMReceiver : public AActor
{
    GENERATED_BODY()

public:
    ADMReceiver();
    ~ADMReceiver();

    UPROPERTY(BlueprintCallable)
    FOnReceiverUpdated Updated;

    bool IsActive() const { return bActive; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bLogMessage = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", meta = (EditCondition = "!bActive"))
    FString SocketName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", meta = (EditCondition = "!bActive"))
    FString IP = TEXT("192.168.0.104");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", meta = (EditCondition = "!bActive"))
    int Port;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", meta = (EditCondition = "!bActive"))
    float ThreadWaitInSeconds = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", meta = (EditCondition = "!bActive"))
    int BufferSizeInBytes = 1024;

private:
    void Initialize();

    FSocket* UdpSocket;
    FUdpSocketReceiver* UdpSocketReceiver;

    void GetMessage(const FArrayReaderPtr& InData, const FIPv4Endpoint& InEndpoint);
};
