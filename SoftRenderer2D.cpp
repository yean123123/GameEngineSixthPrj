
#include "Precompiled.h"
#include "SoftRenderer.h"

// �׸��� �׸���
void SoftRenderer::DrawGrid2D()
{
	// �׸��� ����
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// ���� ���� ���� �׸���
	ScreenPoint screenHalfSize = _ScreenSize.GetHalf();

	for (int x = screenHalfSize.X; x <= _ScreenSize.X; x += _Grid2DUnit)
	{
		_RSI->DrawFullVerticalLine(x, gridColor);
		if (x > screenHalfSize.X)
		{
			_RSI->DrawFullVerticalLine(2 * screenHalfSize.X - x, gridColor);
		}
	}

	for (int y = screenHalfSize.Y; y <= _ScreenSize.Y; y += _Grid2DUnit)
	{
		_RSI->DrawFullHorizontalLine(y, gridColor);
		if (y > screenHalfSize.Y)
		{
			_RSI->DrawFullHorizontalLine(2 * screenHalfSize.Y - y, gridColor);
		}
	}

	// ���� �� �׸���
	_RSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}




// ���� ����
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// ��� �ð� ����
	static float elapsedTime = 0.f;
	elapsedTime += InDeltaSeconds;

	// 1�ʿ� 0~1 ������ �����ϴ� ���� �ĵ� ����� 
	float twoPiUnit = elapsedTime * Math::TwoPI;
	float sinWave = sinf(twoPiUnit);
	float sinWave01 = (sinWave + 1.f) * 0.5f;

	// ���� ����
	_CurrentColor.R = 1.f - sinWave01;
	_CurrentColor.G = 0.f;
	_CurrentColor.B = sinWave01;

	// ũ�� ��� ����
	static float minScale = 0.5f;
	static float maxScale = 2.0f;
	static float scaleLength = maxScale - minScale;
	float currentScale = sinWave01 * scaleLength + minScale;
	Matrix2x2 scaleMatrix = Matrix2x2::Identity * currentScale;

	// ȸ�� ��� ����
	static float rotateSpeed = 10.f;
	static float currentAngle = 0.f;
	currentAngle += rotateSpeed * InDeltaSeconds;
	float currentRadian = Math::Deg2Rad(currentAngle);
	float sin = sinf(currentRadian);
	float cos = cosf(currentRadian);
	Vector2 xAxis(cos, sin);
	Vector2 yAxis(-sin, cos);
	Matrix2x2 rotationMatrix(xAxis, yAxis);

	// �� ����� �����ϱ�. ������ ������ �� 
	_DeltaTransform = rotationMatrix * scaleMatrix;
}

// ������ ����
void SoftRenderer::Render2D()
{
	// ���� �׸���
	DrawGrid2D();

	static Vector2 originVector(100.f, 0.f);

	// 10�� �������� 36���� ���� �׸���.
	for (int i = 0; i < 72; i++)
	{
		float rotateRadian = Math::Deg2Rad(i * 5.f);
		float sin = sinf(rotateRadian);
		float cos = cosf(rotateRadian);

		Vector2 newXAxis(cos, cos);
		Vector2 newYAxis(sin, cos);
		Matrix2x2 rotateMatrix(newXAxis, newYAxis);
		Matrix2x2 finalMatrix = rotateMatrix * _DeltaTransform;
		Vector2 newPosition = finalMatrix * originVector;

		// ������ ��ġ�� ������ �������� �� ���
		_RSI->DrawPoint(newPosition, _CurrentColor);
		_RSI->DrawPoint(newPosition + Vector2::UnitX, _CurrentColor);
		_RSI->DrawPoint(newPosition - Vector2::UnitX, _CurrentColor);
		_RSI->DrawPoint(newPosition + Vector2::UnitY, _CurrentColor);
		_RSI->DrawPoint(newPosition - Vector2::UnitY, _CurrentColor);
		
	}

	// ���� ��ġ�� ȭ�鿡 ���
	_RSI->PushStatisticTexts(_DeltaTransform.ToStrings());
}
