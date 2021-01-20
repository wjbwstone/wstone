#ifndef _SQLITE_ROW_2020_01_14_17_00_46_H
#define _SQLITE_ROW_2020_01_14_17_00_46_H

#include <helper/Types.h>

#include "SqliteCell.h"

namespace ws {

namespace sqlite {

class Row {
public:
	Row() {
		_cells.reserve(20);
	}

	~Row() {
		for(auto it : _cells) {
			delete it;
		}
	}

	inline u32 size() const { return _cells.size(); }
	inline void push(Cell* cell) { _cells.push_back(cell); }
	inline const Cell* get(u32 index) const { return _cells[index]; }

private:
	std::vector<Cell*> _cells;
};

}

}

#endif //_SQLITE_ROW_2020_01_14_17_00_46_H