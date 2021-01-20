#ifndef _LIBMYSQL_ROW_2020_03_19_13_16_15_H
#define _LIBMYSQL_ROW_2020_03_19_13_16_15_H

#include <helper/Types.h>

#include "MySQLCell.h"

namespace ws {

namespace mysql {

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

	const u32 size() const { return _cells.size(); }
	void push(Cell* cell) { _cells.push_back(cell); }
	const Cell* get(u32 index) const { return _cells[index]; }

private:
	std::vector<Cell*> _cells;
};

}

}
#endif //_LIBMYSQL_ROW_2020_03_19_13_16_15_H