struct GSOutput
{
	float4 color : COLOR;
	float4 pos : SV_POSITION;
};

[maxvertexcount(3)]
void main( triangle float4 input[3] : SV_POSITION, 	inout LineStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}