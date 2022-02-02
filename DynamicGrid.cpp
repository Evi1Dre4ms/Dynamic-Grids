#include "DynamicGrid.h"

using namespace serenity;

Grid::Grid(TArray<Grid::ptr>& grids) : rootGrids(grids) {}

Cell::Direction Grid::IndexToDirection(FIntPoint& idx)
{
	if		(idx.X ==  0  && idx.Y ==  1)		return Direction::FRONT;
	else if (idx.X ==  0  && idx.Y == -1)		return Direction::BACK;
	else if (idx.X ==  1  && idx.Y ==  0)		return Direction::LEFT;
	else if (idx.X == -1  && idx.Y ==  0)		return Direction::RIGHT;
	else if (idx.X == -1  && idx.Y ==  1)		return Direction::FRONT_RIGHT;
	else if (idx.X ==  1  && idx.Y ==  1)		return Direction::FRONT_LEFT;
	else if (idx.X == -1  && idx.Y == -1)		return Direction::BACK_RIGHT;
	else if (idx.X ==  1  && idx.Y == -1)		return Direction::BACK_LEFT;

	return Direction::UNDEFINED;
}

Cell::Direction Grid::GetCW(Direction dir)
{
	switch (dir)
	{
	case Direction::FRONT:
		return Direction::FRONT_RIGHT;
		break;

	case Direction::BACK:
		return Direction::BACK_LEFT;
		break;

	case Direction::LEFT:
		return Direction::FRONT_LEFT;
		break;

	case Direction::RIGHT:
		return Direction::BACK_RIGHT;
		break;

	case Direction::FRONT_RIGHT:
		return Direction::RIGHT;
		break;

	case Direction::BACK_RIGHT:
		return Direction::BACK;
		break;

	case Direction::BACK_LEFT:
		return Direction::LEFT;
		break;

	case Direction::FRONT_LEFT:
		return Direction::FRONT;
		break;
	}

	return Direction::UNDEFINED;
}

Cell::Direction Grid::GetCCW(Direction dir)
{
	switch (dir)
	{
	case Direction::FRONT:
		return Direction::FRONT_LEFT;
		break;

	case Direction::BACK:
		return Direction::BACK_RIGHT;
		break;

	case Direction::LEFT:
		return Direction::BACK_LEFT;
		break;

	case Direction::RIGHT:
		return Direction::FRONT_RIGHT;
		break;

	case Direction::FRONT_RIGHT:
		return Direction::FRONT;
		break;

	case Direction::BACK_RIGHT:
		return Direction::RIGHT;
		break;

	case Direction::BACK_LEFT:
		return Direction::BACK;
		break;

	case Direction::FRONT_LEFT:
		return Direction::LEFT;
		break;
	}

	return Direction::UNDEFINED;
}

Cell::Direction Grid::GetOpposite(Direction dir)
{
	switch (dir)
	{
	case Direction::FRONT:
		return Direction::BACK;
		break;

	case Direction::BACK:
		return Direction::FRONT;
		break;

	case Direction::LEFT:
		return Direction::RIGHT;
		break;

	case Direction::RIGHT:
		return Direction::LEFT;
		break;

	case Direction::FRONT_RIGHT:
		return Direction::BACK_LEFT;
		break;

	case Direction::BACK_RIGHT:
		return Direction::FRONT_LEFT;
		break;

	case Direction::BACK_LEFT:
		return Direction::FRONT_RIGHT;
		break;

	case Direction::FRONT_LEFT:
		return Direction::BACK_RIGHT;
		break;
	}

	return Direction::UNDEFINED;
}

bool Grid::Link(Cell::ptr g1, Cell::ptr g2)
{
	if (!IsValid(g1) || !IsValid(g2))
		return false;

	auto dt = g2->GetIndex() - g1->GetIndex();
	
	// If not neighbours
	if (FMath::Abs(dt.X) > 1 || FMath::Abs(dt.Y) > 1)
		return false;

	// note: that means g1 -> DIRECTION -> g2
	Direction dir = IndexToDirection(dt);
	switch (dir)
	{
		// g2 above g1
	case Direction::FRONT:
		if (!g1->GetN(Direction::FRONT)) g1->SetN(Direction::FRONT, g2);
		if (!g2->GetN(Direction::BACK))  g2->SetN(Direction::BACK,  g1);
		break;

		// g2 below g1
	case Direction::BACK:
		if (!g1->GetN(Direction::BACK))  g1->SetN(Direction::BACK,  g2);
		if (!g2->GetN(Direction::FRONT)) g2->SetN(Direction::FRONT, g1);
		break;

		// g2 left from g1
	case Direction::LEFT:
		if (!g1->GetN(Direction::LEFT))  g1->SetN(Direction::LEFT,  g2);
		if (!g2->GetN(Direction::RIGHT)) g2->SetN(Direction::RIGHT, g1);
		break;

		// g2 right from g1
	case Direction::RIGHT:
		if (!g1->GetN(Direction::RIGHT)) g1->SetN(Direction::RIGHT, g2);
		if (!g2->GetN(Direction::LEFT))  g2->SetN(Direction::LEFT,  g1);
		break;

		// g2 above/right g1
	case Direction::FRONT_RIGHT:
		if (!g1->GetN(Direction::FRONT_RIGHT)) g1->SetN(Direction::FRONT_RIGHT, g2);
		if (!g2->GetN(Direction::BACK_LEFT))   g2->SetN(Direction::BACK_LEFT,   g1);
		break;

		// g2 below/right g1
	case Direction::BACK_RIGHT:
		if (!g1->GetN(Direction::BACK_RIGHT))  g1->SetN(Direction::BACK_RIGHT, g2);
		if (!g2->GetN(Direction::FRONT_LEFT))  g2->SetN(Direction::FRONT_LEFT, g1);
		break;

		// g2 below/left g1
	case Direction::BACK_LEFT:
		if (!g1->GetN(Direction::BACK_LEFT))   g1->SetN(Direction::BACK_LEFT,   g2);
		if (!g2->GetN(Direction::FRONT_RIGHT)) g2->SetN(Direction::FRONT_RIGHT, g1);
		break;

		// g2 above/left g1
	case Direction::FRONT_LEFT:
		if (!g1->GetN(Direction::FRONT_LEFT))  g1->SetN(Direction::FRONT_LEFT, g2);
		if (!g2->GetN(Direction::BACK_RIGHT))  g2->SetN(Direction::BACK_RIGHT, g1);
		break;
	}

	return true;
}

bool Grid::LinkNeighbours(Cell::ptr ch)
{
	if (!IsValid(ch)) return false;

	for (int idx = 0; idx < 8; idx++)
	{
		Direction dir = static_cast<Direction>(idx);

		// note: здесь может быть косяк с тем, что если у нас есть 
		// изначально только 1 сосед, то при проходе по нему появятся новые
		// и от последовательности цикла зависит попадет ли новоопределенный сосед в эту проверку или нет.
		// т.е. изначально был единственный back, после него появился right и код пройдётся по этому right.
		auto nb = ch->GetN(dir);
		if (!IsValid(nb)) continue;

		// Common link
		Link(ch, nb);

		switch (dir)
		{
		case Direction::FRONT:
			Link(ch, nb->GetN(Direction::LEFT));
			Link(ch, nb->GetN(Direction::RIGHT));
			Link(ch, nb->GetN(Direction::BACK_LEFT));
			Link(ch, nb->GetN(Direction::BACK_RIGHT));

			Link(nb, ch->GetN(Direction::LEFT));
			Link(nb, ch->GetN(Direction::RIGHT));
			Link(nb, ch->GetN(Direction::FRONT_LEFT));
			Link(nb, ch->GetN(Direction::FRONT_RIGHT));

			break;
			// ch = X, nb - NB
		case Direction::BACK:
			Link(ch, nb->GetN(Direction::LEFT));
			Link(ch, nb->GetN(Direction::RIGHT));
			Link(ch, nb->GetN(Direction::FRONT_LEFT));
			Link(ch, nb->GetN(Direction::FRONT_RIGHT));

			Link(nb, ch->GetN(Direction::LEFT));
			Link(nb, ch->GetN(Direction::RIGHT));
			Link(nb, ch->GetN(Direction::BACK_LEFT));
			Link(nb, ch->GetN(Direction::BACK_RIGHT));
			break;

		case Direction::LEFT:
			Link(ch, nb->GetN(Direction::FRONT));
			Link(ch, nb->GetN(Direction::BACK));
			Link(ch, nb->GetN(Direction::BACK_RIGHT));
			Link(ch, nb->GetN(Direction::FRONT_RIGHT));

			Link(nb, ch->GetN(Direction::FRONT));
			Link(nb, ch->GetN(Direction::BACK));
			Link(nb, ch->GetN(Direction::FRONT_LEFT));
			Link(nb, ch->GetN(Direction::BACK_LEFT));
			break;

		case Direction::RIGHT:
			Link(ch, nb->GetN(Direction::FRONT));
			Link(ch, nb->GetN(Direction::BACK));
			Link(ch, nb->GetN(Direction::BACK_LEFT));
			Link(ch, nb->GetN(Direction::FRONT_LEFT));

			Link(nb, ch->GetN(Direction::FRONT));
			Link(nb, ch->GetN(Direction::BACK));
			Link(nb, ch->GetN(Direction::FRONT_RIGHT));
			Link(nb, ch->GetN(Direction::BACK_RIGHT));
			break;

		case Direction::FRONT_RIGHT:
			Link(ch, nb->GetN(Direction::LEFT));
			Link(ch, nb->GetN(Direction::BACK));

			Link(nb, ch->GetN(Direction::FRONT));
			Link(nb, ch->GetN(Direction::RIGHT));
			break;

		case Direction::BACK_RIGHT:
			Link(ch, nb->GetN(Direction::LEFT));
			Link(ch, nb->GetN(Direction::FRONT));

			Link(nb, ch->GetN(Direction::RIGHT));
			Link(nb, ch->GetN(Direction::BACK));
			break;

		case Direction::BACK_LEFT:
			Link(ch, nb->GetN(Direction::FRONT));
			Link(ch, nb->GetN(Direction::RIGHT));

			Link(nb, ch->GetN(Direction::LEFT));
			Link(nb, ch->GetN(Direction::BACK));
			break;

		case Direction::FRONT_LEFT:
			Link(ch, nb->GetN(Direction::BACK));
			Link(ch, nb->GetN(Direction::RIGHT));

			Link(nb, ch->GetN(Direction::FRONT));
			Link(nb, ch->GetN(Direction::LEFT));
			break;

		}
	}

	return true;
}

Cell::ptr Grid::MakeNeighbour(Cell::ptr ch, Direction dir)
{
	if (!IsValid(ch)) return nullptr;

	Cell::ptr neighbour = Cell::MakeCell();
	cells.Push(neighbour);

	auto idx = ch->GetIndex();
	neighbour->SetIndex(idx + GetPosFromDir(dir));
	Link(ch, neighbour);

	return neighbour;
}

TArray<Cell::ptr> Grid::MakeNeighbours(Cell::ptr& ch)
{
	// For newly created neighbours
	TArray<Cell::ptr> nbs;

	if (IsValid(ch))
	{
		// Iterate all cells around current chunk and create neighbours
		for (int idx = 0; idx < 8; idx++)
		{
			Direction dir = static_cast<Direction>(idx);

			// If neighbour not created -> create
			if (!ch->GetN(dir)) nbs.Push(MakeNeighbour(ch, dir));
		}

		for (auto nb : nbs)
			LinkNeighbours(nb);
	}

	return nbs;
}

Delivered Grid::Resize(int radius)
{
	Delivered delivered;

	if (radius <= 0)
		return delivered;

	auto dt = radius - nRadius;

	if (dt > 0 && nRadius < nLimMax)
		for (int idx = 0; idx < dt; idx++)
		{
			nRadius++;

			delivered += Expand(Direction::FRONT);
			delivered += Expand(Direction::BACK);
			delivered += Expand(Direction::LEFT);
			delivered += Expand(Direction::RIGHT);
		}
	else if (dt < 0 && nRadius > nLimMin)
		for (int idx = 0; idx < std::abs(dt); idx++)
		{
			nRadius--;

			delivered += NarrowDown(Direction::FRONT);
			delivered += NarrowDown(Direction::BACK);
			delivered += NarrowDown(Direction::LEFT);
			delivered += NarrowDown(Direction::RIGHT);
		}

	return delivered;
}

Delivered Grid::Init(int x, int y, int radius)
{
	Delivered delivered;

	if (bIsInit) return delivered;

	if (radius < 1)
		radius = 1;

	if (!IsValid(root))
	{
		root = Cell::MakeCell({ x, y });
		cells.Push(root);
	}
	
	delivered.created.Push(root);
	delivered += Resize(radius);

	return delivered;
}

Delivered Grid::Init(FIntPoint pos, int radius)
{
	return Init(pos.X, pos.Y, radius);
}

Delivered Grid::MoveTo(int x, int y)
{
	Delivered delivered;

	if (root->GetIndex() == FIntPoint(x, y)) return delivered;

	/* Manage cells of this grid that goes to field of another grids
	* 1: First we need to find list of grids which collides with current grid after moving
	*/
	auto CollideGrids = rootGrids.FilterByPredicate([&](const Grid::ptr& g)
		{
			if (this == g.Get()) return false;

			auto root = g->GetRoot();
			if (!IsValid(g->GetRoot()))
				return false;

			// Find difference between positions of two grids
			auto dt = root->GetIndex() - FIntPoint(x, y);
			dt.X = FMath::Abs(dt.X);
			dt.Y = FMath::Abs(dt.Y);

			// TODO: fix
			float r1r2 = this->GetRadius() - 1 + g->GetRadius() - 1;

			// Returns true if [this] inside [g], false if they TOUCH EACH OTHER!
			return dt.X <= r1r2 && dt.Y < r1r2;
		});

	// TEMP
	for (auto grid : CollideGrids)
	{
		check(GEngine != nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Turquoise, "Collided.");

		break;
	}

	// Find difference between new and old positions
	auto dt = FIntPoint(x, y) - root->GetIndex();

	// optimization: Check if shift more than grid radius, so we need to recreate grid (like teleport)
	if (FMath::Abs(dt.X) > nRadius || FMath::Abs(dt.Y) > nRadius)
	{
		auto radius = nRadius;

		// TODO: мега важно исправить, чтобы работало с новым Expand()
		delivered += Clear();
		delivered += Init(x, y, radius);
	}
	// Move grid sequentially by X/Y coordes
	else
	{
		Cell::ptr ch = root;

		// For X axis
		for (int i = 0; i < FMath::Abs(dt.X) && IsValid(ch); i++)
		{
			Dir dir = FMath::Sign(dt.X) > 0 ? Dir::LEFT : Dir::RIGHT;

			if (IsValid(ch->GetN(dir)))
			{
				ch = ch->GetN(dir);

				delivered += NarrowDown(GetOpposite(dir));
				delivered += Expand(dir, CollideGrids);

				root = ch;
			}
		}

		// For Y axis
		for (int i = 0; i < FMath::Abs(dt.Y) && IsValid(ch); i++)
		{
			Dir dir = FMath::Sign(dt.Y) > 0 ? Dir::FRONT : Dir::BACK;

			if (IsValid(ch->GetN(dir)))
			{
				ch = ch->GetN(dir);

				delivered += NarrowDown(GetOpposite(dir));
				delivered += Expand(dir, CollideGrids);

				root = ch;
			}
		}
	}

	/* NOTE: some cells could become invalid after NarrowDown/Expand calls,
	* so we need to remove them here
	*/

	// Check if there is invalid cells
	delivered.created.RemoveAll([&](const Cell::ptr& cc) { return !IsValid(cc); });
	delivered.deleted.RemoveAll([&](const void* cc) { return cc == nullptr; });

	return delivered;
}

Delivered Grid::MoveTo(FIntPoint pos)
{
	return MoveTo(pos.X, pos.Y);
}

Delivered Grid::Clear()
{
	Delivered delivered;

	if (!IsValid(root)) 
		return delivered;

	TArray<Cell::ptr> toDelete;

	// Collect all cells
	auto cornerCells = SelectBorder(Direction::FRONT);
	for (auto cc : cornerCells)
	{
		for (Cell::ptr i = cc; IsValid(i); i = i->GetN(Direction::BACK))
			toDelete.Push(i);
	}

	// Reset cells
	for (auto cc : toDelete)
	{
		// Copy data to deleted
		delivered.deleted.Push(cc->GetData());

		cc->Reset();
	}

	// Reset root
	root.Reset();
	root = nullptr;

	bIsInit = false;
	nRadius = 1;

	return delivered;
}

const FIntPoint Grid::GetPosFromDir(Direction dir)
{
	auto pos = FIntPoint(0, 0);

	switch (dir)
	{
	case Direction::FRONT:
		pos = FIntPoint(0, 1);
		break;

	case Direction::BACK:
		pos = FIntPoint(0, -1);
		break;

	case Direction::LEFT:
		pos = FIntPoint(1, 0);
		break;

	case Direction::RIGHT:
		pos = FIntPoint(-1, 0);
		break;

	case Direction::FRONT_RIGHT:
		pos = FIntPoint(-1, 1);
		break;

	case Direction::BACK_RIGHT:
		pos = FIntPoint(-1, -1);
		break;

	case Direction::BACK_LEFT:
		pos = FIntPoint(1, -1);
		break;

	case Direction::FRONT_LEFT:
		pos = FIntPoint(1, 1);
		break;

	}

	return pos;
}

Cell::ptr Grid::GetRoot()
{
	return root;
}

int Grid::GetRadius() const
{
	return nRadius;
}

Cell::ptr Grid::FindLast(Direction direction)
{
	if (!IsValid(root)) return nullptr;

	Cell::ptr c = root;

	switch (direction)
	{
	case Direction::FRONT:
	case Direction::BACK:
	case Direction::LEFT:
	case Direction::RIGHT:
		for (int i = 0; IsValid(c) && i < this->GetRadius() - 1; i++)
		{
			if (IsValid(c->GetN(direction)))
				c = c->GetN(direction);
			else
			{
				// something wrong
			}
		}
		break;

	case Direction::FRONT_RIGHT: // go to last FRONT, then go to last RIGHT
		c = FindLast(Dir::FRONT);
		for (int i = 0; IsValid(c) && i < this->GetRadius() - 1; i++)
		{
			if (IsValid(c->GetN(Direction::RIGHT)))
				c = c->GetN(Direction::RIGHT);
		}
		break;

	case Direction::BACK_LEFT: // go to last BACK, then go to last LEFT
		c = FindLast(Dir::BACK);
		for (int i = 0; IsValid(c) && i < this->GetRadius() - 1; i++)
		{
			if (IsValid(c->GetN(Direction::LEFT)))
				c = c->GetN(Direction::LEFT);
		}
		break;
	// TODO: 
	case Direction::BACK_RIGHT:
		throw;
		break;
	case Direction::FRONT_LEFT:
		throw;
		break;
	}

	return c;
}

TArray<Grid::ptr> Grid::FindCollidedGrids()
{
	return {};
}

Cell::ptr Grid::FindCellByIndex(FIntPoint index)
{
	auto rootIndex = root->GetIndex();
	auto dt = index - rootIndex;

	// Check if index in grid field
	if (FMath::Abs(dt.X) >= nRadius || FMath::Abs(dt.Y) >= nRadius)
		return nullptr;

	// Find cell
	Cell::ptr c = root;

	// Axis X
	if (FMath::Sign(dt.X) > 0) // To left X+
	{
		for (int i = 0; i < dt.X && IsValid(c); i++)
		{
			if (IsValid(c->GetN(Direction::LEFT)))
				c = c->GetN(Direction::LEFT);
			else
				throw;
		}
	}
	else if (FMath::Sign(dt.X) < 0) // To right X-
	{
		for (int i = 0; i > dt.X && IsValid(c); i--)
		{
			if (IsValid(c->GetN(Direction::RIGHT)))
				c = c->GetN(Direction::RIGHT);
			else
				throw;
		}
	}

	// Axis Y
	if (FMath::Sign(dt.Y) > 0) // To left X+
	{
		for (int i = 0; i < dt.Y && IsValid(c); i++)
		{
			if (IsValid(c->GetN(Direction::FRONT)))
				c = c->GetN(Direction::FRONT);
			else
				throw;
		}
	}
	else if (FMath::Sign(dt.Y) < 0) // To right X-
	{
		for (int i = 0; i > dt.Y && IsValid(c); i--)
		{
			if (IsValid(c->GetN(Direction::BACK)))
				c = c->GetN(Direction::BACK);
			else
				throw;
		}
	}

	return c;
}

TArray<Cell::ptr> Grid::SelectBorder(Direction direction)
{
	TArray<Cell::ptr> borderList;
	if (!IsValid(root))
		throw;

	auto c = root;

	auto dRad = this->GetRadius() * 2 - 1;

	// Find border chunks
	switch (direction)
	{
	case Direction::FRONT:
		c = FindLast(Direction::FRONT_RIGHT);
		for (int i = 0; IsValid(c) && i < dRad; i++)
		{
			borderList.Push(c);

			if (IsValid(c->GetN(Direction::LEFT)))
				c = c->GetN(Direction::LEFT);
			else
				break;
		}
		break;

	case Direction::BACK:
		c = FindLast(Direction::BACK_LEFT);
		for (int i = 0; IsValid(c) && i < dRad; i++)
		{
			borderList.Push(c);

			if (IsValid(c->GetN(Direction::RIGHT)))
				c = c->GetN(Direction::RIGHT);
			else
				break;
		}
		break;

	case Direction::LEFT:
		c = FindLast(Direction::BACK_LEFT);
		for (int i = 0; IsValid(c) && i < dRad; i++)
		{
			borderList.Push(c);

			if (IsValid(c->GetN(Direction::FRONT)))
				c = c->GetN(Direction::FRONT);
			else
				break;
		}
		break;

	case Direction::RIGHT:
		c = FindLast(Direction::FRONT_RIGHT);
		for (int i = 0; IsValid(c) && i < dRad; i++)
		{
			borderList.Push(c);

			if (IsValid(c->GetN(Direction::BACK)))
				c = c->GetN(Direction::BACK);
			else
				break;
		}
		break;

	default:
		return {};
	}

	return borderList;
}

Delivered Grid::Expand(Direction direction)
{
	Delivered delivered;

	if (!IsValid(root)) 
		return delivered;

	// Find border chunks
	TArray<Cell::ptr> border = SelectBorder(direction);

	// Generate new borders and prepare link list
	TArray<Cell::ptr> toLink;
	for (auto cc : border)
		toLink.Push(MakeNeighbour(cc, direction));

	// Link neighbours
	for (auto cc : toLink)
		LinkNeighbours(cc);

	// if (toExpand.Num() != toLink.Num())


	// Fill newly created data
	delivered.created.Append(toLink);

	return delivered;
}

Delivered Grid::Expand(Direction direction, TArray<Grid::ptr>& collideGrids)
{
	Delivered delivered;

	if (!IsValid(root))
		return delivered;

	if (collideGrids.Num())
	{
		// Collect all cells in border
		TArray<Cell::ptr> borderCells = SelectBorder(direction);

		// Collect cell indices to expand
		TMap<FIntPoint, Cell::ptr> toExpandIndices;
		for (auto grid : borderCells)
			toExpandIndices.Add(grid->GetIndex() + GetPosFromDir(direction), grid);

		// нужно или нет?
		TArray<Cell::ptr> toLink;

		// alternate Expand
		for (auto idx : toExpandIndices)
		{
			Cell::ptr cell = nullptr;

			// Find cell in other grids
			for (auto collidedGrid : collideGrids)
			{
				cell = collidedGrid->FindCellByIndex(idx.Key);
				if (IsValid(cell)) break;
			}

			// If exist, link it with current grid
			if (IsValid(cell))
			{
				idx.Value->SetN(direction, cell);
				Link(cell, idx.Value);

				cell->NumOwners()++;
			}
			// Otherwise create cell
			else
				toLink.Push(MakeNeighbour(idx.Value, direction));
		}

		// Link neighbours
		for (auto cc : toLink)
			LinkNeighbours(cc);

		delivered.created.Append(toLink);
	}
	else
		delivered += Expand(direction);

	return delivered;
}

Delivered Grid::NarrowDown(Direction direction)
{
	Delivered delivered;

	if (!IsValid(root)) return delivered;

	// Check direction
	Direction inversedDir = Direction::UNDEFINED;
	switch (direction)
	{
	case Direction::FRONT:
		inversedDir = Direction::BACK;
		break;

	case Direction::BACK:
		inversedDir = Direction::FRONT;
		break;

	case Direction::LEFT:
		inversedDir = Direction::RIGHT;
		break;

	case Direction::RIGHT:
		inversedDir = Direction::LEFT;
		break;

	// if direction came wrong
	default:
		return delivered;
		break;
	}

	TArray<Cell::ptr> toUnlink;

	// Find border chunks
	TArray<Cell::ptr> toRemove = SelectBorder(direction);
	for (auto cc : toRemove)
	{
		// Если есть другой владелец, то уменьшаем кол-во и скип
		if (cc->NumOwners() > 1)
		{
			cc->NumOwners()--;
			continue;
		}

		if (!IsValid(cc)) continue;

		toUnlink.Push(cc->GetN(inversedDir));
		delivered.deleted.Push(cc->GetData());
		
		// Reset current cell
		cc->Reset();
	}

	// Unlink not valid chunks
	// TODO: maybe some optimization
	for (auto cc : toUnlink)
	{
		if (!IsValid(cc)) continue;

		// Unlink
		for (int idx = 0; idx < 8; idx++)
		{
			Direction dir = static_cast<Direction>(idx);

			// If not valid -> reset pointers for 'cc'
			if (!cc->GetN(dir))
				cc->SetN(dir, nullptr);
		}
	}

	return delivered;
}

//////////////////////////////////////////////////////////////////////////

Cell::Cell() : index(0, 0) {}

Cell::ptr Cell::MakeCell(FIntPoint index)
{
	Cell* new_cell = new Cell();
	new_cell->SetIndex(index.X, index.Y);
	new_cell->bIsValid = true;

	return MakeShareable(new_cell);
}

FIntPoint Cell::GetIndex()
{
	return index;
}

void Cell::SetIndex(int x, int y)
{
	index = FIntPoint(x, y);
}

void Cell::SetIndex(FIntPoint pos)
{
	SetIndex(pos.X, pos.Y);
}

void*& Cell::GetData()
{
	return pMetaData;
}

size_t& Cell::NumOwners()
{
	return nNumOwners;
}

bool Cell::IsValid() const
{
	return bIsValid;
}

void Cell::Reset()
{
	// Reset all pointers to nullptr
	Front.Reset();
	FrontR.Reset();
	FrontL.Reset();
	Right.Reset();
	Left.Reset();
	Back.Reset();
	BackL.Reset();
	BackR.Reset();

	// Reset data
	// pMetaData.reset();

	bIsValid = false;
	bIsReseted = true;
}

Cell::ptr Cell::GetN(Direction dir)
{
	Cell::ptr* tmp = nullptr;

	switch (dir)
	{
	case Direction::FRONT:
		tmp = &Front;
		break;
	case Direction::BACK:
		tmp = &Back;
		break;
	case Direction::LEFT:
		tmp = &Left;
		break;
	case Direction::RIGHT:
		tmp = &Right;
		break;
	case Direction::FRONT_RIGHT:
		tmp = &FrontR;
		break;
	case Direction::BACK_RIGHT:
		tmp = &BackR;
		break;
	case Direction::BACK_LEFT:
		tmp = &BackL;
		break;
	case Direction::FRONT_LEFT:
		tmp = &FrontL;
		break;
	}

	// If cell was reseted before, so return null
	if (tmp->IsValid() && !serenity::IsValid(*tmp))
	{
		(*tmp)->Reset();
		tmp->Reset();
	}

	return *tmp;
}

void Cell::SetN(Direction dir, Cell::ptr g)
{
	switch (dir)
	{
	case Direction::FRONT:
		Front = g;
		break;
	case Direction::BACK:
		Back = g;
		break;
	case Direction::LEFT:
		Left = g;
		break;
	case Direction::RIGHT:
		Right = g;
		break;
	case Direction::FRONT_RIGHT:
		FrontR = g;
		break;
	case Direction::BACK_RIGHT:
		BackR = g;
		break;
	case Direction::BACK_LEFT:
		BackL = g;
		break;
	case Direction::FRONT_LEFT:
		FrontL = g;
		break;

	default:
		break;
	}

	return;
}

////////////////////////////////////////////////////////////////

bool serenity::IsValid(Cell::ptr p)
{
	return p.IsValid() && p->IsValid();
}

GridManager::GridManager() {}

Grid::ptr GridManager::CreateGrid()
{
	Grid::ptr New = MakeShareable(new Grid(rootGrids));
	// New->Init(x, y, num_waves);

	rootGrids.Push(New);
	return New;
}

bool GridManager::DestroyGrid(Grid::ptr g)
{
	return (bool)rootGrids.Remove(g);;
}

GridManager::~GridManager() 
{
	/*for (auto grid : rootGrids)
		if (grid) grid.Reset();*/
}