#pragma once

#include "AccelerometerDriver.h"

// �����, ������������ ������� �� ���������� ��������� � ������������
template <uint8_t SEGMENT_COUNT>
class SegmentIdentifier
{
public:
	// ���������, ����������� ������� � ������������ 
	struct Segment
	{
		AccelerometerDriver::Position PositionStart;	// ������� ������ ��������
		AccelerometerDriver::Position PositionEnd;		// ������� ��������� ��������
		float Hysteresis;								// ���������� �������� ����� ����������, ������� 
	};

	// �����������
	// [in] segments - ������ ���������
	SegmentIdentifier(Segment segments[SEGMENT_COUNT])
	{
		memcpy(Segments, segments, SEGMENT_COUNT * sizeof(Segment));
		currentSegment = 0;
	}

	// ���������� ����� �������� ��� ���������� ��������� � ������ �����������, 
	// ���� UINT8_MAX, ���� ��������� �� ������������� �� ������ ��������
	uint8_t GetCurrentSegment(AccelerometerDriver::Position position)
	{
		// ���������, ����� �� �� ������� ����������� ��������
		// ���� ����� �� ������� �������� ��������, �� ���� ������� � ������� ������,
		// ����� ���������� ������� �������
		if (!isPreviosSegment(position))
		{
			// ���� ����� �������
			currentSegment = searchNewSegment(position);
			return currentSegment;
		}
		else
		{
			return currentSegment;
		}
	}

	// ���������� ����� ������� �������
	uint8_t CurrentSegment()
	{
		return currentSegment;
	}

private:
	Segment Segments[SEGMENT_COUNT];
	uint8_t currentSegment;

	bool isPreviosSegment(AccelerometerDriver::Position position)
	{
		// ������� �������� ��������
		Segment segment = Segments[currentSegment];
		
		bool x = (
			( position.xAngle >= min(segment.PositionStart.xAngle + segment.Hysteresis, 
									 segment.PositionEnd.xAngle - segment.Hysteresis) ) &&
			( position.xAngle <= max(segment.PositionStart.xAngle + segment.Hysteresis,
									 segment.PositionEnd.xAngle - segment.Hysteresis) )
		);
		bool y = (
			( position.yAngle >= min(segment.PositionStart.yAngle - segment.Hysteresis, 
									 segment.PositionEnd.yAngle + segment.Hysteresis) ) &&
			( position.yAngle <= max(segment.PositionStart.yAngle - segment.Hysteresis,
									 segment.PositionEnd.yAngle + segment.Hysteresis) )
		);
		bool z = (
			( position.zAngle >= min(segment.PositionStart.zAngle, segment.PositionEnd.zAngle) ) &&
			( position.zAngle <= max(segment.PositionStart.zAngle, segment.PositionEnd.zAngle) )
		);

		if (x && y && z)
			return true;
		else
			return false;
	}

	uint8_t searchNewSegment(AccelerometerDriver::Position position)
	{
		for (uint8_t i = 0; i < SEGMENT_COUNT; i++)
		{
			if (isInRange(Segments[i].PositionStart, Segments[i].PositionEnd, position))
				return i;
		}
		return UINT8_MAX;
	}

	bool isInRange(AccelerometerDriver::Position positionStart,
		AccelerometerDriver::Position positionEnd,
		AccelerometerDriver::Position position)
	{
		bool x = (
			(position.xAngle >= min(positionStart.xAngle, positionEnd.xAngle)) &&
			(position.xAngle <= max(positionStart.xAngle, positionEnd.xAngle))
			);
		bool y = (
			(position.yAngle >= min(positionStart.yAngle, positionEnd.yAngle)) &&
			(position.yAngle <= max(positionStart.yAngle, positionEnd.yAngle))
			);
		bool z = (
			(position.zAngle >= min(positionStart.zAngle, positionEnd.zAngle)) &&
			(position.zAngle <= max(positionStart.zAngle, positionEnd.zAngle))
			);

		return x && y && z;
	}
};