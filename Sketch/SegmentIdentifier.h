#pragma once

#include "AccelerometerDriver.h"

// Класс, определяющий сегмент по указанному положению в пространстве
template <uint8_t SEGMENT_COUNT>
class SegmentIdentifier
{
public:
	// Структура, описывающая сегмент в пространстве 
	struct Segment
	{
		AccelerometerDriver::Position PositionStart;	// Позиция начала сегмента
		AccelerometerDriver::Position PositionEnd;		// Позиция окончания сегмента
		float Hysteresis;								// Гистерезис перехода между сегментами, градусы 
	};

	// Конструктор
	// [in] segments - массив сегментов
	SegmentIdentifier(Segment segments[SEGMENT_COUNT])
	{
		memcpy(Segments, segments, SEGMENT_COUNT * sizeof(Segment));
		currentSegment = 0;
	}

	// Возвращает номер сегмента для указанного положения с учетом гистерезиса, 
	// либо UINT8_MAX, если положение не соответствует ни одному сегменту
	uint8_t GetCurrentSegment(AccelerometerDriver::Position position)
	{
		// Проверить, вышли ли за границу предыдущего сегмента
		// Если вышли за границу текущего сегмента, то ищем сегмент в который попали,
		// иначе возвращаем текущий сегмент
		if (!isPreviosSegment(position))
		{
			// Ищем новый сегмент
			currentSegment = searchNewSegment(position);
			return currentSegment;
		}
		else
		{
			return currentSegment;
		}
	}

	// Возвращает номер текущий сегмент
	uint8_t CurrentSegment()
	{
		return currentSegment;
	}

private:
	Segment Segments[SEGMENT_COUNT];
	uint8_t currentSegment;

	bool isPreviosSegment(AccelerometerDriver::Position position)
	{
		// Позиция прошлого сегмента
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