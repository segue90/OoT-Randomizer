import re
import sys
from typing import TYPE_CHECKING, Dict, Pattern, Callable, Any

if TYPE_CHECKING:
    from State import State

# The better way to type hint the access rule requires Python 3.8.
if sys.version_info >= (3, 8):
    from typing import Protocol

    class AccessRule(Protocol):
        def __call__(self, state: "State", **kwargs):
            ...
else:
    from Utils import TypeAlias
    AccessRule: TypeAlias = Callable[["State"], bool]


# Variable names and values used by rule execution,
# will be automatically filled by Items
allowed_globals: Dict[str, Any] = {}


_escape: Pattern[str] = re.compile(r'[\'()[\]-]')


def escape_name(name: str) -> str:
    return _escape.sub('', name.replace(' ', '_'))
