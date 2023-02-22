from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from Hints import HintArea
    from Item import Item
    from Region import Region
    from World import World


class Dungeon:
    def __init__(self, world: World, name: str, hint: HintArea) -> None:
        self.world: World = world
        self.name: str = name
        self.hint: HintArea = hint
        self.regions: list[Region] = []
        self.boss_key: list[Item] = []
        self.small_keys: list[Item] = []
        self.dungeon_items: list[Item] = []
        self.silver_rupees: list[Item] = []

        for region in world.regions:
            if region.dungeon == self.name:
                region.dungeon = self
                self.regions.append(region)

    def copy(self, new_world: World) -> Dungeon:
        new_dungeon = Dungeon(new_world, self.name, self.hint)

        new_dungeon.boss_key = [item.copy(new_world) for item in self.boss_key]
        new_dungeon.small_keys = [item.copy(new_world) for item in self.small_keys]
        new_dungeon.dungeon_items = [item.copy(new_world) for item in self.dungeon_items]
        new_dungeon.silver_rupees = [item.copy(new_world) for item in self.silver_rupees]

        return new_dungeon

    @property
    def keys(self) -> list[Item]:
        return self.small_keys + self.boss_key

    @property
    def all_items(self) -> list[Item]:
        return self.dungeon_items + self.keys + self.silver_rupees

    def item_name(self, text: str) -> str:
        return f"{text} ({self.name})"

    def get_silver_rupee_names(self) -> set[str]:
        from Item import ItemInfo
        return {name for name, item in ItemInfo.items.items() if item.type == 'SilverRupee' and self.name in name}

    def get_item_names(self) -> set[str]:
        return (self.get_silver_rupee_names() |
                {self.item_name(name) for name in ["Map", "Compass", "Small Key", "Boss Key", "Small Key Ring"]})

    def is_dungeon_item(self, item: Item) -> bool:
        return item.name in [dungeon_item.name for dungeon_item in self.all_items]

    def __str__(self) -> str:
        return str(self.__unicode__())

    def __unicode__(self) -> str:
        return '%s' % self.name
