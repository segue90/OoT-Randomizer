import math
import operator
from typing import Any, Optional


# holds the info for a single setting
class SettingInfo:
    def __init__(self, setting_type: type, gui_text: Optional[str], gui_type: Optional[str], shared: bool, choices=None, default=None, disabled_default=None, disable=None, gui_tooltip=None, gui_params=None, cosmetic: bool = False) -> None:
        self.type: type = setting_type  # type of the setting's value, used to properly convert types to setting strings
        self.shared: bool = shared  # whether the setting is one that should be shared, used in converting settings to a string
        self.cosmetic: bool = cosmetic  # whether the setting should be included in the cosmetic log
        self.gui_text: Optional[str] = gui_text
        self.gui_type: Optional[str] = gui_type
        self.gui_tooltip: Optional[str] = "" if gui_tooltip is None else gui_tooltip
        self.gui_params: dict = {} if gui_params is None else gui_params  # additional parameters that the randomizer uses for the gui
        self.disable: dict = disable  # dictionary of settings this setting disabled
        self.dependency = None  # lambda that determines if this is disabled. Generated later

        # dictionary of options to their text names
        choices = {} if choices is None else choices
        if isinstance(choices, list):
            self.choices: dict = {k: k for k in choices}
            self.choice_list: list = list(choices)
        else:
            self.choices: dict = dict(choices)
            self.choice_list: list = list(choices.keys())
        self.reverse_choices: dict = {v: k for k, v in self.choices.items()}

        # number of bits needed to store the setting, used in converting settings to a string
        if shared:
            if self.gui_params.get('min') and self.gui_params.get('max') and not choices:
                self.bitwidth = math.ceil(math.log(self.gui_params.get('max') - self.gui_params.get('min') + 1, 2))
            else:
                self.bitwidth = self.calc_bitwidth(choices)
        else:
            self.bitwidth = 0

        # default value if undefined/unset
        self.default = default
        if self.default is None:
            if self.type == bool:
                self.default = False
            elif self.type == str:
                self.default = ""
            elif self.type == int:
                self.default = 0
            elif self.type == list:
                self.default = []
            elif self.type == dict:
                self.default = {}

        # default value if disabled
        self.disabled_default = self.default if disabled_default is None else disabled_default

        # used to when random options are set for this setting
        if 'distribution' not in self.gui_params:
            self.gui_params['distribution'] = [(choice, 1) for choice in self.choice_list]

    def __set_name__(self, owner, name: str) -> None:
        self.name = name

    def __get__(self, obj, obj_type=None) -> Any:
        return obj.settings_dict.get(self.name, self.default)

    def __set__(self, obj, value: Any) -> None:
        obj.settings_dict[self.name] = value

    def __delete__(self, obj) -> None:
        del obj.settings_dict[self.name]

    def calc_bitwidth(self, choices) -> int:
        count = len(choices)
        if count > 0:
            if self.type == list:
                # Need two special values for terminating additive and subtractive lists
                count = count + 2
            return math.ceil(math.log(count, 2))
        return 0

    def create_dependency(self, disabling_setting: 'SettingInfo', option, negative: bool = False) -> None:
        op = operator.__ne__ if negative else operator.__eq__
        if self.dependency is None:
            self.dependency = lambda settings: op(getattr(settings, disabling_setting.name), option)
        else:
            old_dependency = self.dependency
            self.dependency = lambda settings: op(getattr(settings, disabling_setting.name), option) or old_dependency(settings)


class SettingInfoNone(SettingInfo):
    def __init__(self, gui_text: Optional[str], gui_type: Optional[str], gui_tooltip=None, gui_params=None) -> None:
        super().__init__(type(None), gui_text, gui_type, False, None, None, None, None, gui_tooltip, gui_params, False)

    def __get__(self, obj, obj_type=None) -> None:
        raise Exception(f"{self.name} is not a setting and cannot be retrieved.")

    def __set__(self, obj, value: str) -> None:
        raise Exception(f"{self.name} is not a setting and cannot be set.")


class SettingInfoBool(SettingInfo):
    def __init__(self, gui_text: Optional[str], gui_type: Optional[str], shared: bool, default=None, disabled_default=None, disable=None, gui_tooltip=None, gui_params=None, cosmetic: bool = False) -> None:
        choices = {
            True:  'checked',
            False: 'unchecked',
        }

        super().__init__(bool, gui_text, gui_type, shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)

    def __get__(self, obj, obj_type=None) -> bool:
        value = super().__get__(obj, obj_type)
        if not isinstance(value, bool):
            value = bool(value)
        return value

    def __set__(self, obj, value: bool) -> None:
        if not isinstance(value, bool):
            value = bool(value)
        super().__set__(obj, value)


class SettingInfoStr(SettingInfo):
    def __init__(self, gui_text: Optional[str], gui_type: Optional[str], shared: bool = False, choices=None, default=None, disabled_default=None, disable=None, gui_tooltip=None, gui_params=None, cosmetic: bool = False) -> None:
        super().__init__(str, gui_text, gui_type, shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)

    def __get__(self, obj, obj_type=None) -> str:
        value = super().__get__(obj, obj_type)
        if not isinstance(value, str):
            value = str(value)
        return value

    def __set__(self, obj, value: str) -> None:
        if not isinstance(value, str):
            value = str(value)
        super().__set__(obj, value)


class SettingInfoInt(SettingInfo):
    def __init__(self, gui_text: Optional[str], gui_type: Optional[str], shared: bool, choices=None, default=None, disabled_default=None, disable=None, gui_tooltip=None, gui_params=None, cosmetic: bool = False) -> None:
        super().__init__(int, gui_text, gui_type, shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)

    def __get__(self, obj, obj_type=None) -> int:
        value = super().__get__(obj, obj_type)
        if not isinstance(value, int):
            value = int(value)
        return value

    def __set__(self, obj, value: int) -> None:
        if not isinstance(value, int):
            value = int(value)
        super().__set__(obj, value)


class SettingInfoList(SettingInfo):
    def __init__(self, gui_text: Optional[str], gui_type: Optional[str], shared: bool, choices=None, default=None, disabled_default=None, disable=None, gui_tooltip=None, gui_params=None, cosmetic: bool = False) -> None:
        super().__init__(list, gui_text, gui_type, shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)

    def __get__(self, obj, obj_type=None) -> list:
        value = super().__get__(obj, obj_type)
        if not isinstance(value, list):
            value = list(value)
        return value

    def __set__(self, obj, value: list) -> None:
        if not isinstance(value, list):
            value = list(value)
        super().__set__(obj, value)


class SettingInfoDict(SettingInfo):
    def __init__(self, gui_text: Optional[str], gui_type: Optional[str], shared: bool, choices=None, default=None, disabled_default=None, disable=None, gui_tooltip=None, gui_params=None, cosmetic: bool = False) -> None:
        super().__init__(dict, gui_text, gui_type, shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)

    def __get__(self, obj, obj_type=None) -> dict:
        value = super().__get__(obj, obj_type)
        if not isinstance(value, dict):
            value = dict(value)
        return value

    def __set__(self, obj, value: dict) -> None:
        if not isinstance(value, dict):
            value = dict(value)
        super().__set__(obj, value)


class Button(SettingInfoNone):
    def __init__(self, gui_text: Optional[str], gui_tooltip=None, gui_params=None) -> None:
        super().__init__(gui_text, "Button", gui_tooltip, gui_params)


class Textbox(SettingInfoNone):
    def __init__(self, gui_text: Optional[str], gui_tooltip=None, gui_params=None) -> None:
        super().__init__(gui_text, "Textbox", gui_tooltip, gui_params)


class Checkbutton(SettingInfoBool):
    def __init__(self, gui_text: Optional[str], gui_tooltip: Optional[str] = None, disable=None, disabled_default=None, default=False, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'Checkbutton', shared, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class Combobox(SettingInfoStr):
    def __init__(self, gui_text, choices, default, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'Combobox', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class Radiobutton(SettingInfoStr):
    def __init__(self, gui_text, choices, default, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'Radiobutton', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class Fileinput(SettingInfoStr):
    def __init__(self, gui_text, choices=None, default=None, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'Fileinput', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class Directoryinput(SettingInfoStr):
    def __init__(self, gui_text, choices=None, default=None, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'Directoryinput', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class Textinput(SettingInfoStr):
    def __init__(self, gui_text, choices=None, default=None, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'Textinput', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class ComboboxInt(SettingInfoInt):
    def __init__(self, gui_text, choices, default, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'Combobox', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class Scale(SettingInfoInt):
    def __init__(self, gui_text, default, minimum, maximum, step=1, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        choices = {
            i: str(i) for i in range(minimum, maximum+1, step)
        }

        if gui_params is None:
            gui_params = {}
        gui_params['min'] = minimum
        gui_params['max'] = maximum
        gui_params['step'] = step

        super().__init__(gui_text, 'Scale', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class Numberinput(SettingInfoInt):
    def __init__(self, gui_text, default, minimum=None, maximum=None, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        if gui_params is None:
            gui_params = {}
        if minimum is not None:
            gui_params['min'] = minimum
        if maximum is not None:
            gui_params['max'] = maximum

        super().__init__(gui_text, 'Numberinput', shared, None, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class MultipleSelect(SettingInfoList):
    def __init__(self, gui_text, choices, default, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'MultipleSelect', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)


class SearchBox(SettingInfoList):
    def __init__(self, gui_text, choices, default, gui_tooltip=None, disable=None, disabled_default=None, shared=False, gui_params=None, cosmetic=False):
        super().__init__(gui_text, 'SearchBox', shared, choices, default, disabled_default, disable, gui_tooltip, gui_params, cosmetic)
