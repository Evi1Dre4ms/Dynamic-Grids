# Dynamic-Grids

# Introduce

This is a library that operates with grids and cells in 2D space, allowing you to store data in cells and implement logic between them.
The main feature is that when two or more grids intersect, they share the cell data in the corresponding coordinates that allows to avoid data duplication.

For example, this can be used on a game server with a generated world, where it is necessary to load only those chunks(map) of the world that are in the player's field of view. Thus, this allows you not to load the entire world map, but only those that are relevant.
