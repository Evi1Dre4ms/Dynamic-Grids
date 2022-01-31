#pragma once
#include <memory>
#include "CoreMinimal.h"

namespace serenity
{
	class Cell
	{
	public:
		typedef TSharedPtr<Cell> ptr;
		typedef TWeakPtr<Cell> w_ptr;

		enum class Direction : uint8_t
		{
			FRONT,
			BACK,
			LEFT,
			RIGHT,
			FRONT_RIGHT,
			BACK_RIGHT,
			BACK_LEFT,
			FRONT_LEFT,
			UNDEFINED
		};

		static Cell::ptr MakeCell(FIntPoint index = FIntPoint(0, 0));

	public:
		Cell();

		FIntPoint GetIndex();
		void SetIndex(int x, int y);
		void SetIndex(FIntPoint pos);

		Cell::ptr GetN(Direction dir);
		void SetN(Direction dir, Cell::ptr g);

		void*& GetData();

		size_t NumOwners();
		
		bool IsValid() const;

		void Reset();

		bool bIsReseted = false;

	protected:
		
		
		bool bIsValid = false;

		size_t nNumOwners = 0;

		void* pMetaData = nullptr;

		FIntPoint index;

		Cell::ptr Front		= nullptr; //Y+
		Cell::ptr Back		= nullptr; //Y-
		Cell::ptr Left		= nullptr; //X+
		Cell::ptr Right		= nullptr; //X-
		Cell::ptr FrontR	= nullptr;
		Cell::ptr BackR		= nullptr;
		Cell::ptr BackL		= nullptr;
		Cell::ptr FrontL	= nullptr;
	};

	struct Delivered
	{
		TArray<Cell::ptr> created;
		TArray<void*> deleted;

		friend Delivered& operator+= (Delivered& dst, const Delivered& src)
		{
			dst.created.Append(src.created);
			dst.deleted.Append(src.deleted);
			return dst;
		}
	};

	class Grid
	{
	public:
		typedef Cell::Direction Direction;
		typedef Cell::Direction Dir;
		typedef TSharedPtr<Grid> ptr;

	public:

		Grid(TArray<Grid::ptr>& grids);

		// Creates grid by entered world position with relevant radius
		Delivered Init(int x, int y, int radius);
		Delivered Init(FIntPoint pos, int radius);

		Delivered IncreaseRadius();
		Delivered DecreaseRadius();

		// Move the grid to needed position in world coords
		Delivered MoveTo(int x, int y);
		Delivered MoveTo(FIntPoint pos);

		// Creates or deletes cells depending on the new radius 
		Delivered Resize(int radius);

		// Reset all cells and removes them
		Delivered Clear();

		// Returns current root cell
		Cell::ptr GetRoot();

		int GetRadius() const;

	private:

		Cell::ptr root = nullptr;
		int nRadius = 1;
		bool bIsInit = false;
		
		int nLimMax = 16;
		const int nLimMin = 1;

		//TODO: вести список "выпавших" из области чанков и ими инициализировать новую область, 
		// вместо того, чтобы удалять полностью структуру через delete
		Delivered Expand(Direction direction);
		Delivered NarrowDown(Direction direction);

		// Cell::ptr MakeCell(FIntPoint index = FIntPoint());

		bool Link(Cell::ptr g1, Cell::ptr g2);
		bool LinkNeighbours(Cell::ptr g);

		Cell::ptr MakeNeighbour(Cell::ptr g, Direction dir);
		TArray<Cell::ptr> MakeNeighbours(Cell::ptr& g);
		TArray<Cell::ptr> SelectBorder(Direction direction);

		Cell::ptr FindLast(Direction direction);
		Direction IndexToDirection(FIntPoint& idx);
		
		// Get direction clockwise
		Direction GetCW(Direction dir);

		// Get direction counterclockwise
		Direction GetCCW(Direction dir);

		TArray<Grid::ptr>& rootGrids;

		// tmp
		TArray<Cell::w_ptr> cells;
	};

	class GridManager
	{
	public:
		typedef Cell::Direction Direction;

		GridManager();
		~GridManager();

		Grid::ptr CreateGrid();
		bool DestroyGrid(Grid::ptr g);

		// TODO: move function

	protected:

		TArray<Grid::ptr> rootGrids;
	};

	// Checks if cell usable
	bool IsValid(Cell::ptr p);
}

