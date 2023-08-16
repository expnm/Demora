// exp.dev free

#include "DMReceiver.h"

DEFINE_LOG_CATEGORY_STATIC(LogDMReceiver, All, All);

ADMReceiver::ADMReceiver()
{

#if WITH_EDITOR
    if (GEditor)
    {
        GEditor->GetTimerManager()->SetTimerForNextTick(this, &ADMReceiver::Initialize);
    }
#endif // WITH_EDITOR
}

ADMReceiver::~ADMReceiver()
{
    if (UdpSocketReceiver != nullptr) delete UdpSocketReceiver;
    if (UdpSocket != nullptr) delete UdpSocket;
}

void ADMReceiver::BeginPlay()
{
    Super::BeginPlay();

#if !WITH_EDITOR
    Initialize();
#endif // !WITH_EDITOR
}

void ADMReceiver::Initialize()
{
    if (!bActive) return;

    if (SocketName.IsEmpty() || IP.IsEmpty())
    {
        UE_LOG(LogDMReceiver, Error, TEXT("Fill socket info!"));
        return;
    }

    FIPv4Address UdpAddress;
    FIPv4Address::Parse(IP, UdpAddress);
    FIPv4Endpoint Endpoint(UdpAddress, Port);

    UdpSocket = FUdpSocketBuilder(SocketName)
                    .AsReusable()
                    .BoundToEndpoint(Endpoint)
                    .WithBroadcast()
                    .WithReceiveBufferSize(BufferSizeInBytes)
                    .Build();

    FTimespan ThreadWaitTime = FTimespan::FromSeconds(ThreadWaitInSeconds);
    UdpSocketReceiver = new FUdpSocketReceiver(UdpSocket, ThreadWaitTime, *SocketName);
    UdpSocketReceiver->OnDataReceived().BindUObject(this, &ADMReceiver::GetMessage);
    UdpSocketReceiver->Start();
}

void ADMReceiver::GetMessage(const FArrayReaderPtr& InData, const FIPv4Endpoint& InEndpoint)
{
    uint8_t* Buffer = InData->GetData();
    size_t Size = InData->Num();

    FMemory::Memcpy(Buffer, InData->GetData(), InData->Num());
    FString Message = (const char*)Buffer;
    Message.RemoveAt(Size, Message.Len() - Size);

    if (bLogMessage) UE_LOG(LogDMReceiver, Display, TEXT("%s (%s:%d): %s"), *SocketName, *IP, Port, *Message);
    Updated.Broadcast(Message);
}
