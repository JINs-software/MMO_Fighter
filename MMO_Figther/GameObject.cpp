#include "GameObject.h"
#include "ExceptionHandler.h"

void Grid::Add(stObjectInfo* obj) {
	uint16 cellY = obj->stPos.usY / dfGridCell_Length;
	uint16 cellX = obj->stPos.usX / dfGridCell_Length;

	obj->prevGridObj = nullptr;
	obj->nextGridObj = cells[cellY][cellX];
	cells[cellY][cellX] = obj;
	if (obj->nextGridObj != nullptr) {
		obj->nextGridObj->prevGridObj = obj;
	}
}

void Grid::Delete(stObjectInfo* obj) {
	uint16 cellY = obj->stPos.usY / dfGridCell_Length;
	uint16 cellX = obj->stPos.usX / dfGridCell_Length;

	if (cells[cellY][cellX] == nullptr) {
		// ������ �� �ǵ����� ���� �б�
		DebugBreak();
		return;
	}
	else {
		bool delFlag = false;
		if (cells[cellY][cellX] == obj) {
			cells[cellY][cellX] = obj->nextGridObj;
			if (cells[cellY][cellX] != nullptr) {
				cells[cellY][cellX]->prevGridObj = nullptr;
			}
			delFlag = true;
		}
		else {
			stObjectInfo* objptr = cells[cellY][cellX]->nextGridObj;
			while (objptr != nullptr) {
				if (objptr == obj) {
					objptr->prevGridObj->nextGridObj = objptr->nextGridObj;
					if (objptr->nextGridObj != nullptr) {
						objptr->nextGridObj->prevGridObj = objptr->prevGridObj;
					}
					delFlag = true;
					break;
				}

				objptr = objptr->nextGridObj;
			}
		}

		if (!delFlag) {
			// ������ �� �ǵ����� ���� �б�
			DebugBreak();
		}
	}

	obj->prevGridObj = nullptr;
	obj->nextGridObj = nullptr;
}