#Much of this is heavily inspired from and/or based on az64's / Deathbasket's MM randomizer

import itertools
import random
import os
import zipfile
import io
from Utils import compare_version, data_path

# Format: (Title, Sequence ID)
bgm_sequence_ids = (
    ("Hyrule Field", 0x02),
    ("Dodongos Cavern", 0x18),
    ("Kakariko Adult", 0x19),
    ("Battle", 0x1A),
    ("Boss Battle", 0x1B),
    ("Inside Deku Tree", 0x1C),
    ("Market", 0x1D),
    ("Title Theme", 0x1E),
    ("House", 0x1F),
    ("Jabu Jabu", 0x26),
    ("Kakariko Child", 0x27),
    ("Fairy Fountain", 0x28),
    ("Zelda Theme", 0x29),
    ("Fire Temple", 0x2A),
    ("Forest Temple", 0x2C),
    ("Castle Courtyard", 0x2D),
    ("Ganondorf Theme", 0x2E),
    ("Lon Lon Ranch", 0x2F),
    ("Goron City", 0x30),
    ("Miniboss Battle", 0x38),
    ("Temple of Time", 0x3A),
    ("Kokiri Forest", 0x3C),
    ("Lost Woods", 0x3E),
    ("Spirit Temple", 0x3F),
    ("Horse Race", 0x40),
    ("Ingo Theme", 0x42),
    ("Fairy Flying", 0x4A),
    ("Deku Tree", 0x4B),
    ("Windmill Hut", 0x4C),
    ("Shooting Gallery", 0x4E),
    ("Sheik Theme", 0x4F),
    ("Zoras Domain", 0x50),
    ("Shop", 0x55),
    ("Chamber of the Sages", 0x56),
    ("Ice Cavern", 0x58),
    ("Kaepora Gaebora", 0x5A),
    ("Shadow Temple", 0x5B),
    ("Water Temple", 0x5C),
    ("Gerudo Valley", 0x5F),
    ("Potion Shop", 0x60),
    ("Kotake and Koume", 0x61),
    ("Castle Escape", 0x62),
    ("Castle Underground", 0x63),
    ("Ganondorf Battle", 0x64),
    ("Ganon Battle", 0x65),
    ("Fire Boss", 0x6B),
    ("Mini-game", 0x6C)
)

fanfare_sequence_ids = (
    ("Game Over", 0x20),
    ("Boss Defeated", 0x21),
    ("Item Get", 0x22),
    ("Ganondorf Appears", 0x23),
    ("Heart Container Get", 0x24),
    ("Treasure Chest", 0x2B),
    ("Spirit Stone Get", 0x32),
    ("Heart Piece Get", 0x39),
    ("Escape from Ranch", 0x3B),
    ("Learn Song", 0x3D),
    ("Epona Race Goal", 0x41),
    ("Medallion Get", 0x43),
    ("Zelda Turns Around", 0x51),
    ("Master Sword", 0x53),
    ("Door of Time", 0x59)
)

ocarina_sequence_ids = (
    ("Prelude of Light", 0x25),
    ("Bolero of Fire", 0x33),
    ("Minuet of Forest", 0x34),
    ("Serenade of Water", 0x35),
    ("Requiem of Spirit", 0x36),
    ("Nocturne of Shadow", 0x37),
    ("Saria's Song", 0x44),
    ("Epona's Song", 0x45),
    ("Zelda's Lullaby", 0x46),
    ("Sun's Song", 0x47),
    ("Song of Time", 0x48),
    ("Song of Storms", 0x49)
)

class Bank(object):
    def __init__(self, index, meta, data):
        self.index = index
        self.meta = meta
        self.data = data
        self.zsounds = {}

    def add_zsound(self, tempaddr, zsound):
        self.zsounds[tempaddr] = zsound

    def get_entry(self, offset):
        bank_entry = offset.to_bytes(4, 'big')
        bank_entry += len(self.data).to_bytes(4, 'big')
        bank_entry += self.meta
        return bank_entry

    def update_zsound_pointers(self):
        for zsound_tempaddr in self.zsounds.keys():
            self.data = self.data.replace(zsound_tempaddr.to_bytes(4, byteorder='big'), self.zsounds[zsound_tempaddr]['offset'].to_bytes(4, byteorder='big'))

# Represents the information associated with a sequence, aside from the sequence data itself
class Sequence(object):
    def __init__(self, name, cosmetic_name, type = 0x0202, instrument_set = 0x03, replaces = -1, vanilla_id = -1, seq_file = None, new_instrument_set = False, zsounds = None):
        self.name = name
        self.seq_file = seq_file
        self.cosmetic_name = cosmetic_name
        self.replaces = replaces
        self.vanilla_id = vanilla_id
        self.type = type
        self.new_instrument_set = new_instrument_set
        self.zsounds = zsounds
        self.zbank_file = None
        self.bankmeta = None

        if isinstance(instrument_set, str):
            if instrument_set == '-':
                self.instrument_set = 0x0
                self.new_instrument_set = True
            else:
                self.instrument_set = int(instrument_set, 16)
        else:
            self.instrument_set = instrument_set

    def copy(self):
        copy = Sequence(self.name, self.cosmetic_name, self.type, self.instrument_set, self.replaces, self.vanilla_id, self.seq_file, self.new_instrument_set, self.zsounds)
        copy.zbank_file = self.zbank_file
        copy.bankmeta = self.bankmeta
        return copy


# Represents actual sequence data, along with metadata for the sequence data block
class SequenceData(object):
    def __init__(self):
        self.address = -1
        self.size = -1
        self.data = []


def process_sequences(rom, ids, seq_type='bgm', disabled_source_sequences=None, disabled_target_sequences=None, include_custom=True, sequences=None, target_sequences=None, groups=None, include_custom_audiobanks=False):
    disabled_source_sequences = [] if disabled_source_sequences is None else disabled_source_sequences
    disabled_target_sequences = {} if disabled_target_sequences is None else disabled_target_sequences
    sequences = {} if sequences is None else sequences
    target_sequences = {} if target_sequences is None else target_sequences
    groups = {} if groups is None else groups

    # Process vanilla music data
    for bgm in ids:
        # Get sequence metadata
        name = bgm[0]
        cosmetic_name = name
        type = rom.read_int16(0xB89AE8 + (bgm[1] * 0x10))
        instrument_set = rom.read_byte(0xB89911 + 0xDD + (bgm[1] * 2))
        id = bgm[1]

        # Create new sequences
        seq = Sequence(name, cosmetic_name, type, instrument_set, vanilla_id = id)
        target = Sequence(name, cosmetic_name, type, instrument_set, replaces = id)

        # Special handling for file select/fairy fountain
        if seq.vanilla_id != 0x57 and cosmetic_name not in disabled_source_sequences:
            sequences[seq.cosmetic_name] = seq
        if cosmetic_name not in disabled_target_sequences:
            target_sequences[target.cosmetic_name] = target

    if not include_custom:
        return sequences, target_sequences, groups

    # If present, load the file containing custom music to exclude
    try:
        with open(os.path.join(data_path(), u'custom_music_exclusion.txt')) as excl_in:
            seq_exclusion_list = excl_in.readlines()
        seq_exclusion_list = [seq.rstrip() for seq in seq_exclusion_list if seq[0] != '#']
        seq_exclusion_list = [seq for seq in seq_exclusion_list if seq.endswith('.ootrs')]
    except FileNotFoundError:
        seq_exclusion_list = []

    # Process music data in data/Music/
    # Each sequence requires a valid .ootrs zip file
    # .ootrs files are just .zip files containing at minimum:
    #   .seq sequence file
    #   .meta metadata file
    # And optionally .zbank, .bankmeta, and .zsound files

    for dirpath, _, filenames in os.walk(os.path.join(data_path(), 'Music'), followlinks=True):
        for fname in filenames:
            # Skip if included in exclusion file
            if fname in seq_exclusion_list:
                continue

            # Find .ootrs zip files
            if fname.endswith('.ootrs'):
                skip = False
                # Open zip file
                filepath = os.path.join(dirpath, fname)
                with zipfile.ZipFile(filepath) as zip:
                    # Make sure meta file and seq file exists
                    meta_file = None
                    seq_file = None
                    zbank_file = None
                    bankmeta_file = None
                    for f in zip.namelist():
                        if f.endswith(".meta"):
                            meta_file = f
                            continue
                        if f.endswith(".seq"):
                            seq_file = f
                            continue
                        if f.endswith(".zbank"):
                            if not include_custom_audiobanks: # Check if we are excluding sequences with custom banks
                                skip = True
                                break
                            zbank_file = f
                            continue
                        if f.endswith(".bankmeta"):
                            bankmeta_file = f
                            continue

                    if skip:
                        continue

                    if not meta_file:
                        raise FileNotFoundError(f'No .meta file in: "{fname}". This should never happen')
                    if not seq_file:
                        raise FileNotFoundError(f'No .seq file in: "{fname}". This should never happen')
                    if zbank_file and not bankmeta_file:
                        raise FileNotFoundError(f'Custom track "{fname}" contains .zbank but no .bankmeta')

                    # Read meta info
                    try:
                        with zip.open(meta_file, 'r') as stream:
                            lines = io.TextIOWrapper(stream).readlines() # Use TextIOWrapper in order to get text instead of binary from the seq.
                        # Strip newline(s)
                        lines = [line.rstrip() for line in lines]
                    except Exception as ex:
                        raise FileNotFoundError(f'Error reading meta file for: "{fname}". This should never happen')

                    # Create new sequence, checking third line for correct type
                    if (len(lines) > 2 and (lines[2].lower() == seq_type.lower() or lines[2] == '')) or (len(lines) <= 2 and seq_type == 'bgm'):
                        seq = Sequence(filepath, lines[0], seq_file = seq_file, instrument_set = lines[1])
                        if zbank_file:
                            seq.zbank_file = zbank_file
                            seq.bankmeta = bankmeta_file
                        seq.zsounds = []
                        if seq.instrument_set < 0x00 or seq.instrument_set > 0x25:
                            raise Exception(f'{seq.name}: Sequence instrument must be in range [0x00, 0x25]')
                        if seq.cosmetic_name == "None":
                            raise Exception(f'{seq.name}: Sequences should not be named "None" as that is used for disabled music.')
                        if seq.cosmetic_name in sequences:
                            raise Exception(f'{seq.name} Sequence names should be unique. Duplicate sequence name: {seq.cosmetic_name}')

                        if seq.cosmetic_name not in disabled_source_sequences:
                            sequences[seq.cosmetic_name] = seq

                        if len(lines) >= 4:
                            seq_groups = lines[3].split(',')
                            for group in seq_groups:
                                group = group.strip()
                                if group not in groups:
                                    groups[group] = []
                                groups[group].append(seq.cosmetic_name)

                        # Process ZSOUND lines. Make these lines in the format of ZSOUND:file_path:temp_addr
                        for line in lines:
                            tokens = line.split(":")
                            if tokens[0] == "ZSOUND":
                                zsound_file = tokens[1]
                                zsound_tempaddr = tokens[2]
                                zsound = {
                                    "file": tokens[1],
                                    "tempaddr": tokens[2]
                                }
                                seq.zsounds.append(zsound)


    return sequences, target_sequences, groups


def shuffle_music(log, source_sequences, target_sequences, music_mapping, type="music"):
    sequences = []
    favorites = log.src_dict.get('bgm_groups', {}).get('favorites', []).copy()

    if not source_sequences:
        raise Exception(f"Not enough custom {type} ({len(source_sequences)}) to omit base Ocarina of Time sequences ({len(target_sequences)}).")

    # Shuffle the sequences
    sequence_ids = [name for name in source_sequences.keys() if name not in music_mapping.values()]
    random.shuffle(sequence_ids)
    sequence_ids.sort(key=lambda name: name not in favorites)
    del sequence_ids[len([name for name in target_sequences.keys() if name not in music_mapping]):]
    random.shuffle(sequence_ids)

    refill_needed = False
    for name, target_sequence in target_sequences.items():
        if target_sequence.cosmetic_name not in music_mapping:
            # If we've run out of sequences to place, refill sequence_ids
            if not sequence_ids:
                refill_needed = True
                sequence_ids = [name for name in source_sequences.keys()]
                random.shuffle(sequence_ids)
            sequence = source_sequences[sequence_ids.pop()].copy()
        else:
            sequence = source_sequences[music_mapping[target_sequence.cosmetic_name]].copy()
        sequences.append(sequence)
        sequence.replaces = target_sequence.replaces
        log.bgm[target_sequence.cosmetic_name] = sequence.cosmetic_name

    if refill_needed:
        log.errors.append(f"Not enough {type} available to not have repeats. There were {len(source_sequences)} sequences available to fill {len(target_sequences)} target tracks.")
    return sequences


def rebuild_sequences(rom, sequences, log, symbols):
    replacement_dict = {seq.replaces: seq for seq in sequences}
    # List of sequences (actual sequence data objects) containing the vanilla sequence data
    old_sequences = []

    for i in range(0x6E):
        # Create new sequence object, an entry for the audio sequence
        entry = SequenceData()
        # Get the address for the entry's pointer table entry
        entry_address = 0xB89AE0 + (i * 0x10)
        # Extract the info from the pointer table entry
        entry.address = rom.read_int32(entry_address)
        entry.size = rom.read_int32(entry_address + 0x04)

        # If size > 0, read the sequence data from the rom into the sequence object
        if entry.size > 0:
            entry.data = rom.read_bytes(entry.address + 0x029DE0, entry.size)
        else:
            seq = replacement_dict.get(i, None)
            if seq and 0 < entry.address < 128:
                if seq.replaces != 0x28:
                    seq.replaces = entry.address
                else:
                    # Special handling for file select/fairy fountain
                    entry.data = old_sequences[0x57].data
                    entry.size = old_sequences[0x57].size

        old_sequences.append(entry)

    # List of sequences containing the new sequence data
    new_sequences = []
    address = 0
    # Byte array to hold the data for the whole audio sequence
    new_audio_sequence = []

    for i in range(0x6E):
        new_entry = SequenceData()
        # If sequence size is 0, the address doesn't matter and it doesn't effect the current address
        if old_sequences[i].size == 0:
            new_entry.address = old_sequences[i].address
        # Continue from the end of the new sequence table
        else:
            new_entry.address = address

        seq = replacement_dict.get(i, None)
        if seq:
            # If we are using a vanilla sequence, get its data from old_sequences
            if seq.vanilla_id != -1:
                new_entry.size = old_sequences[seq.vanilla_id].size
                new_entry.data = old_sequences[seq.vanilla_id].data
            else:
                # Read sequence info
                try:
                    with zipfile.ZipFile(seq.name) as zip:
                        with zip.open(seq.seq_file, 'r') as stream:
                            new_entry.data = bytearray(stream.read())
                            new_entry.size = len(new_entry.data)
                    if new_entry.size <= 0x10:
                        raise Exception(f'Invalid sequence file "{seq.name}.seq"')
                    new_entry.data[1] = 0x20
                except FileNotFoundError as ex:
                    raise FileNotFoundError(f'No sequence file for: "{seq.name}"')
        else:
            new_entry.size = old_sequences[i].size
            new_entry.data = old_sequences[i].data

        new_sequences.append(new_entry)

        # Concatenate the full audio sequence and the new sequence data
        if new_entry.data != [] and new_entry.size > 0:
            # Align sequences to 0x10
            if new_entry.size % 0x10 != 0:
                new_entry.data.extend(bytearray(0x10 - (new_entry.size % 0x10)))
                new_entry.size += 0x10 - (new_entry.size % 0x10)
            new_audio_sequence.extend(new_entry.data)
            # Increment the current address by the size of the new sequence
            address += new_entry.size

    new_address = 0x029DE0
    # Check if the new audio sequence is larger than the vanilla one
    if address > 0x04F690:
        # Zero out the old audio sequence
        rom.buffer[0x029DE0 : 0x029DE0 + 0x04F690] = [0] * 0x04F690

        # Find free space and update dmatable
        new_address = rom.free_space()
        dma_addr = rom.update_dmadata_record(0x029DE0, new_address, new_address + address)

    # Write new audio sequence file
    rom.write_bytes(new_address, new_audio_sequence)

    # Update pointer table
    for i in range(0x6E):
        rom.write_int32(0xB89AE0 + (i * 0x10), new_sequences[i].address)
        rom.write_int32(0xB89AE0 + (i * 0x10) + 0x04, new_sequences[i].size)
        seq = replacement_dict.get(i, None)
        if seq:
            rom.write_int16(0xB89AE0 + (i * 0x10) + 0x08, seq.type)

    # Update instrument sets
    for i in range(0x6E):
        base = 0xB89911 + 0xDD + (i * 2)
        j = replacement_dict.get(i if new_sequences[i].size else new_sequences[i].address, None)
        if j:
            rom.write_byte(base, j.instrument_set)


    # Patch new instrument sets (banks) and add new instrument sounds
    # Only if we were passed CFG_AUDIOBANK_TABLE_EXTENDED_ADDR via symbols which means we're on the right version.
    if not "CFG_AUDIOBANK_TABLE_EXTENDED_ADDR" in symbols.keys():
        return
    added_banks = [] # Store copies of all of the banks we've added
    added_instruments = [] #Store copies of all of the instruments we've added
    new_bank_index = 0x26
    instr_data = bytearray(0) # Store all of the new instrument data that will be added to the end of audiotable

    audiobank_start, audiobank_end, audiobank_size = rom.get_dmadata_record_by_key(0xD390)
    audiotable_start, audiotable_end, audiotable_size = rom.get_dmadata_record_by_key(0x79470)

    instr_offset_in_file = audiotable_size
    for i in range(0x6E):
        bank_table_base = (rom.read_int32(symbols['CFG_AUDIOBANK_TABLE_EXTENDED_ADDR']) - 0x80400000) + 0x3480000
        seq_bank_base = 0xB89911 + 0xDD + (i * 2)
        j = replacement_dict.get(i if new_sequences[i].size else new_sequences[i].address, None)
        if(j is not None and j.new_instrument_set):
            # Open the .ootrs file
            with zipfile.ZipFile(j.name) as zip:

                # Load the .zbank file
                with zip.open(j.zbank_file, 'r') as stream:
                    bankdata = stream.read()
                    bank = None

                # Check if we have already added this bank
                for added_bank in added_banks:
                    if added_bank.data == bankdata:
                        bank = added_bank

                if not bank:
                    bank_meta = bytearray(zip.open(j.bankmeta, 'r').read())
                    bank = Bank(new_bank_index, bank_meta, bankdata)

                    # Handle any new instruments
                    for zsound in j.zsounds:
                        instrument = None
                        tempaddr = int(zsound["tempaddr"], 16)
                        curr_instrument_data = zip.open(zsound["file"], 'r').read()
                        already_added = False
                        for added_instrument in added_instruments:
                            if(added_instrument['data'] == curr_instrument_data):
                                # Already added this instrument. Just add it to the bank
                                instrument = added_instrument
                                bank.add_zsound(tempaddr, instrument)
                                already_added = True
                        if not already_added:
                            instrument = {}
                            instrument['offset'] = instr_offset_in_file
                            instrument['data'] = curr_instrument_data
                            instrument['size'] = len(curr_instrument_data)
                            instrument['name'] = zsound["file"]
                            instr_data += curr_instrument_data

                            # Align instrument data to 0x10
                            if len(instr_data) % 0x10 != 0:
                                padding_length = 0x10 - (len(instr_data) % 0x10)
                                instr_data += (bytearray(padding_length))
                                instrument['size'] += padding_length
                            bank.add_zsound(tempaddr, instrument)
                            added_instruments.append(instrument)
                            instr_offset_in_file += instrument['size']
                    added_banks.append(bank)
                    new_bank_index += 1

                # Update the sequence's bank (instrument set)
                rom.write_byte(seq_bank_base, bank.index)

    # Patch the new instrument data into the ROM in a new file.
    # If there is any instrument data to add, move the entire audiotable file to a new location in the ROM.
    if len(instr_data) > 0:
        # Read the original audiotable data
        audiotable_data = rom.read_bytes(audiotable_start, audiotable_size)
        # Zeroize existing file
        rom.write_bytes(audiotable_start, [0] * audiotable_size)
        # Get new address for the file
        new_audiotable_start = rom.free_space()
        # Add the new data
        audiotable_data += instr_data
        # Write the file to the new address
        rom.write_bytes(new_audiotable_start, audiotable_data)
        # Update DMA
        instr_dma_index = rom.update_dmadata_record(audiotable_start, new_audiotable_start, new_audiotable_start + len(audiotable_data))
        log.instr_dma_index = instr_dma_index

    # Add new audio banks
    new_bank_data = bytearray(0)
    # Read the original audiobank data
    audiobank_data = rom.read_bytes(audiobank_start, audiobank_size)
    new_bank_offset = len(audiobank_data)
    for bank in added_banks:
        bank.update_zsound_pointers()
        bank.offset = new_bank_offset
        #absolute_offset = new_audio_banks_addr + new_bank_offset
        bank_entry = bank.get_entry(new_bank_offset)
        rom.write_bytes(bank_table_base + 0x10 + bank.index * 0x10, bank_entry)
        new_bank_data += bank.data
        new_bank_offset += len(bank.data)

    # If we have new banks to add, move the entire audiobank file to a new place in ROM. Update the existing dmadata record
    if len(new_bank_data) > 0:
        # Zeroize existing file
        rom.write_bytes(audiobank_start, [0] * audiobank_size)
        # Get new address for the file
        new_audio_banks_addr = rom.free_space()
        # Add the new data
        audiobank_data += new_bank_data
        # Write the file to the new address
        rom.write_bytes(new_audio_banks_addr, audiobank_data)
        # Update DMA
        bank_dma_index = rom.update_dmadata_record(audiobank_start, new_audio_banks_addr, new_audio_banks_addr + len(audiobank_data))
        log.bank_dma_index = new_audio_banks_addr
        # Update size of bank table in the Audiobank table header.
        rom.write_bytes(bank_table_base, new_bank_index.to_bytes(2, 'big'))

    # Update the init heap size. This size is normally hardcoded based on the number of audio banks.
    init_heap_size = rom.read_int32(0xB80118)
    init_heap_size += (new_bank_index - 0x26)*0x20
    rom.write_int32(0xB80118, init_heap_size)
    log.added_banks = added_banks

def rebuild_pointers_table(rom, sequences, log, symbols):
    for sequence in [s for s in sequences if s.vanilla_id and s.replaces]:
        bgm_sequence = rom.original.read_bytes(0xB89AE0 + (sequence.vanilla_id * 0x10), 0x10)
        bgm_instrument = rom.original.read_int16(0xB89910 + 0xDD + (sequence.vanilla_id * 2))
        rom.write_bytes(0xB89AE0 + (sequence.replaces * 0x10), bgm_sequence)
        rom.write_int16(0xB89910 + 0xDD + (sequence.replaces * 2), bgm_instrument)

    # Write Fairy Fountain instrument to File Select (uses same track but different instrument set pointer for some reason)
    rom.write_int16(0xB89910 + 0xDD + (0x57 * 2), rom.read_int16(0xB89910 + 0xDD + (0x28 * 2)))


def randomize_music(rom, settings, log, symbols):
    shuffled_sequences = shuffled_fanfare_sequences = []
    sequences = fanfare_sequences = target_sequences = target_fanfare_sequences = bgm_groups = fanfare_groups = {}
    disabled_source_sequences = log.src_dict.get('bgm_groups', {}).get('exclude', []).copy()
    disabled_target_sequences = {}
    music_mapping = log.src_dict.get('bgm', {}).copy()
    bgm_ids = {bgm[0]: bgm for bgm in bgm_sequence_ids}
    ff_ids = {bgm[0]: bgm for bgm in fanfare_sequence_ids}
    ocarina_ids = {bgm[0]: bgm for bgm in ocarina_sequence_ids}

    # If generating a patch file, disallow custom sequences.
    custom_sequences_enabled = not settings.generating_patch_file
    custom_audiobanks_enabled = True
    if not custom_sequences_enabled and (settings.background_music == 'random_custom_only' or settings.fanfares == 'random_custom_only'):
        log.errors.append("Custom music is disabled when creating patch files. Only randomizing vanilla music.")

    # If generating from patch, do a version check to make sure custom sequences are supported.
    if settings.patch_file != '':
        rom_version_bytes = rom.read_version_bytes()
        rom_version = f'{rom_version_bytes[0]}.{rom_version_bytes[1]}.{rom_version_bytes[2]}'
        if compare_version(rom_version, '7.2.0') < 0: # Check if custom sequences with custom banks are supported
            custom_audiobanks_enabled = False
        if compare_version(rom_version, '4.11.13') < 0:
            log.errors.append("Custom music is not supported by this patch version. Only randomizing vanilla music.")
            custom_sequences_enabled = False

    # Check if we have mapped music for BGM, Fanfares, or Ocarina Fanfares
    bgm_mapped = any(name in music_mapping for name in bgm_ids)
    ff_mapped = any(name in music_mapping for name in ff_ids)
    ocarina_mapped = any(name in music_mapping for name in ocarina_ids)

    # Flag sequence locations that are set to off for disabling.
    disabled_ids = []
    if settings.background_music == 'off':
        disabled_ids += [music_id for music_id in bgm_ids.values()]
    if settings.fanfares == 'off':
        disabled_ids += [music_id for music_id in ff_ids.values()]
        if settings.ocarina_fanfares:
            disabled_ids += [music_id for music_id in ocarina_ids.values()]
    for bgm in itertools.chain(bgm_ids.values(), ff_ids.values(), ocarina_ids.values()):
        if music_mapping.get(bgm[0], '') == "None":
            disabled_target_sequences[bgm[0]] = bgm
            del music_mapping[bgm[0]]
    for bgm in disabled_ids:
        if bgm[0] not in music_mapping:
            disabled_target_sequences[bgm[0]] = bgm

    # Map music to itself if music is set to normal.
    normal_ids = []
    if settings.background_music == 'normal' and bgm_mapped:
        normal_ids += [music_id for music_id in bgm_ids.values()]
    if settings.fanfares == 'normal' and (ff_mapped or ocarina_mapped):
        normal_ids += [music_id for music_id in ff_ids.values()]
    if settings.fanfares == 'normal' and ocarina_mapped:
        normal_ids += [music_id for music_id in ocarina_ids.values()]
    for bgm in normal_ids:
        if bgm[0] not in music_mapping:
            music_mapping[bgm[0]] = bgm[0]

    # Include ocarina songs in fanfare pool if checked
    if settings.ocarina_fanfares or ocarina_mapped:
        ff_ids.update(ocarina_ids)

    # Grab our lists of sequences.
    if settings.background_music in ['random', 'random_custom_only'] or bgm_mapped:
        sequences, target_sequences, bgm_groups = process_sequences(rom, bgm_ids.values(), 'bgm', disabled_source_sequences, disabled_target_sequences, custom_sequences_enabled, include_custom_audiobanks=custom_audiobanks_enabled)
        if settings.background_music == 'random_custom_only':
            sequences = {name: seq for name, seq in sequences.items() if name not in bgm_ids or name in music_mapping.values()}

    if settings.fanfares in ['random', 'random_custom_only'] or ff_mapped or ocarina_mapped:
        fanfare_sequences, target_fanfare_sequences, fanfare_groups = process_sequences(rom, ff_ids.values(), 'fanfare', disabled_source_sequences, disabled_target_sequences, custom_sequences_enabled, include_custom_audiobanks=custom_audiobanks_enabled)
        if settings.fanfares == 'random_custom_only':
            fanfare_sequences = {name: seq for name, seq in fanfare_sequences.items() if name not in ff_ids or name in music_mapping.values()}

    # Handle groups.
    plando_groups = {n: s for n, s in log.src_dict.get('bgm_groups', {}).get('groups', {}).items()}
    bgm_groups_full = chain_groups([{n: s} for n, s in itertools.chain(bgm_groups.items(), plando_groups.items())], sequences)
    ff_groups_full = chain_groups([{n: s} for n, s in itertools.chain(fanfare_groups.items(), plando_groups.items())], fanfare_sequences)
    bgm_groups = {n: s.copy() for n, s in bgm_groups_full.items()}
    ff_groups = {n: s.copy() for n, s in ff_groups_full.items()}
    for target, mapping in music_mapping.copy().items():
        if target in bgm_ids:
            groups_full_alias = bgm_groups_full
            groups_alias = bgm_groups
            sequences_alias = sequences
        elif target in ff_ids:
            groups_full_alias = ff_groups_full
            groups_alias = ff_groups
            sequences_alias = fanfare_sequences
        else:
            log.error.append(f'Target sequence "{target}" from plando file is invalid.')
            del music_mapping[target]
            continue

        source = mapping
        group = group_name = None
        if isinstance(mapping, list):
            # Try to find a valid source in the defined list
            while len(mapping) > 0:
                random.shuffle(mapping)
                source = music_mapping[target] = mapping.pop()

                if source.startswith('#'):
                    group_name = source[1:]
                    group = groups_alias.get(group_name, None)

                    # Check if group exists.
                    if group is not None:
                        # Check if we need to refill this group from the source dictionary.
                        if not group:
                            groups_alias[group_name] = groups_full_alias.get(group_name, []).copy()
                            group = groups_alias[group_name]

                        if group:
                            random.shuffle(group)
                            source = music_mapping[target] = group.pop()
                            if source in sequences_alias:
                                break

                    log.errors.append(f"Warning: Group '{source}' linked to '{target}' does not have a valid custom sequence. Ignoring!")
                else:
                    break

            if len(mapping) == 0 and source not in sequences_alias:
                del music_mapping[target]
                log.errors.append(f"Target Sequence '{target}' does not have a valid 'bgm_groups' entry.")
                continue

        elif mapping.startswith('#'):
            group_name = source[1:]
            group = groups_alias.get(group_name, None)

            # Check if group exists.
            if group is not None:
                # Check if we need to refill this group from the source dictionary.
                if not group:
                    groups_alias[group_name] = groups_full_alias.get(group_name, []).copy()
                    group = groups_alias[group_name]

                if group:
                    random.shuffle(group)
                    source = music_mapping[target] = group.pop()

        # Check if mapped sequence exists.
        if source not in sequences_alias:
            del music_mapping[target]
            log.errors.append(f"Sequence '{source}' mapped to '{target}' was not found.")

    # Shuffle the sequences.
    if sequences and target_sequences:
        shuffled_sequences = shuffle_music(log, sequences, target_sequences, music_mapping)
    if fanfare_sequences and target_fanfare_sequences:
        shuffled_fanfare_sequences = shuffle_music(log, fanfare_sequences, target_fanfare_sequences, music_mapping, "fanfares")

    # Patch the randomized sequences into the ROM.
    patch_music = rebuild_sequences if custom_sequences_enabled else rebuild_pointers_table
    patch_music(rom, shuffled_sequences + shuffled_fanfare_sequences, log, symbols)

    if disabled_target_sequences:
        disable_music(rom, log, disabled_target_sequences.values())


def disable_music(rom, log, ids):
    # First track is no music
    blank_track = rom.read_bytes(0xB89AE0 + (0 * 0x10), 0x10)
    for bgm in ids:
        rom.write_bytes(0xB89AE0 + (bgm[1] * 0x10), blank_track)
        log.bgm[bgm[0]] = "None"


def restore_music(rom):
    # Restore all music from original
    for bgm in bgm_sequence_ids + fanfare_sequence_ids + ocarina_sequence_ids:
        bgm_sequence = rom.original.read_bytes(0xB89AE0 + (bgm[1] * 0x10), 0x10)
        rom.write_bytes(0xB89AE0 + (bgm[1] * 0x10), bgm_sequence)
        bgm_instrument = rom.original.read_int16(0xB89910 + 0xDD + (bgm[1] * 2))
        rom.write_int16(0xB89910 + 0xDD + (bgm[1] * 2), bgm_instrument)

    # restore file select instrument
    bgm_instrument = rom.original.read_int16(0xB89910 + 0xDD + (0x57 * 2))
    rom.write_int16(0xB89910 + 0xDD + (0x57 * 2), bgm_instrument)

    # Rebuild audioseq
    orig_start, orig_end, orig_size = rom.original._get_dmadata_record(0x7470)
    rom.write_bytes(orig_start, rom.original.read_bytes(orig_start, orig_size))

    # If Audioseq was relocated
    start, end, size = rom._get_dmadata_record(0x7470)
    if start != 0x029DE0:
        # Zero out old audioseq
        rom.write_bytes(start, [0] * size)
        rom.update_dmadata_record(start, orig_start, orig_end)


def chain_groups(group_list, sequences):
    result = {}
    for iterator in group_list:
        for n, s in iterator.items():
            if isinstance(s, list):
                result.setdefault(n, []).extend(ns for ns in s if ns in sequences)
            else:
                result.setdefault(n, []).append(ns for ns in s if ns in sequences)
    return result

