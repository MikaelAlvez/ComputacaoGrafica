struct PixelIn
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(PixelIn pIn) : SV_TARGET
{
    return float4( pIn.Color * (1 - pIn.PosH.zzzz));
}