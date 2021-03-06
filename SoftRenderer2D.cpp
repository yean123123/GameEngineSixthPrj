
#include "Precompiled.h"
#include "SoftRenderer.h"

// 그리드 그리기
void SoftRenderer::DrawGrid2D()
{
	// 그리드 색상
	LinearColor gridColor(LinearColor(0.8f, 0.8f, 0.8f, 0.3f));

	// 가로 세로 라인 그리기
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

	// 월드 축 그리기
	_RSI->DrawFullHorizontalLine(screenHalfSize.Y, LinearColor::Red);
	_RSI->DrawFullVerticalLine(screenHalfSize.X, LinearColor::Green);
}




// 게임 로직
void SoftRenderer::Update2D(float InDeltaSeconds)
{
	// 경과 시간 저장
	static float elapsedTime = 0.f;
	elapsedTime += InDeltaSeconds;

	// 1초에 0~1 범위로 진동하는 사인 파동 만들기 
	float twoPiUnit = elapsedTime * Math::TwoPI;
	float sinWave = sinf(twoPiUnit);
	float sinWave01 = (sinWave + 1.f) * 0.5f;

	// 색상 변경
	_CurrentColor.R = 1.f - sinWave01;
	_CurrentColor.G = 0.f;
	_CurrentColor.B = sinWave01;

	// 크기 행렬 생성
	static float minScale = 0.5f;
	static float maxScale = 2.0f;
	static float scaleLength = maxScale - minScale;
	float currentScale = sinWave01 * scaleLength + minScale;
	Matrix2x2 scaleMatrix = Matrix2x2::Identity * currentScale;

	// 회전 행렬 생성
	static float rotateSpeed = 10.f;
	static float currentAngle = 0.f;
	currentAngle += rotateSpeed * InDeltaSeconds;
	float currentRadian = Math::Deg2Rad(currentAngle);
	float sin = sinf(currentRadian);
	float cos = cosf(currentRadian);
	Vector2 xAxis(cos, sin);
	Vector2 yAxis(-sin, cos);
	Matrix2x2 rotationMatrix(xAxis, yAxis);

	// 두 행렬을 조합하기. 순서에 주의할 것 
	_DeltaTransform = rotationMatrix * scaleMatrix;
}

// 렌더링 로직
void SoftRenderer::Render2D()
{
	// 격자 그리기
	DrawGrid2D();

	static Vector2 originVector(100.f, 0.f);

	// 10도 간격으로 36개의 점을 그린다.
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

		// 지정된 위치에 지정한 색상으로 점 찍기
		_RSI->DrawPoint(newPosition, _CurrentColor);
		_RSI->DrawPoint(newPosition + Vector2::UnitX, _CurrentColor);
		_RSI->DrawPoint(newPosition - Vector2::UnitX, _CurrentColor);
		_RSI->DrawPoint(newPosition + Vector2::UnitY, _CurrentColor);
		_RSI->DrawPoint(newPosition - Vector2::UnitY, _CurrentColor);
		
	}

	// 현재 위치를 화면에 출력
	_RSI->PushStatisticTexts(_DeltaTransform.ToStrings());
}
