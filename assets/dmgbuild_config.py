#!/usr/bin/env python3
#
# Copyright (C) 2019 Adrian Carpenter
# 
# Multiplatform Qt Deployment Tool
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

from __future__ import unicode_literals

import os.path

if "defines" not in globals():
    exit(1)

sourceRoot = defines.get('sourceRoot')
buildArch = defines.get('buildArch')

application = f'{sourceRoot}/bin/{buildArch}/Deploy/Regular Expressions 101.app'

appname = os.path.basename(application)

format = 'UDBZ'

size = None

files = [ application ]

symlinks = { 'Applications': '/Applications' }

badge_icon = f'{sourceRoot}/assets/regex101.icns'

icon_locations = {
    appname:        (199, 276),
    'Applications': (569, 276)
}

background = f'{sourceRoot}/assets/dmg_background@2x.tiff'

show_status_bar = False
show_tab_view = False
show_toolbar = False
show_pathbar = False
show_sidebar = False
sidebar_width = 180
image_width = 768
image_height = 512

window_rect = ((100+image_width, 100+image_height), (image_width, image_height))

default_view = 'icon-view'

show_icon_preview = False

include_icon_view_settings = 'auto'
include_list_view_settings = 'auto'

arrange_by = None
grid_offset = (0, 0)
grid_spacing = 100
scroll_position = (0, 0)
label_pos = 'bottom' # or 'right'
text_size = 16
icon_size = 160

list_icon_size = 16
list_text_size = 12
list_scroll_position = (0, 0)
list_sort_by = 'name'
list_use_relative_dates = True
list_calculate_all_sizes = False,
list_columns = ('name', 'date-modified', 'size', 'kind', 'date-added')

list_column_widths = {
    'name': 300,
    'date-modified': 181,
    'date-created': 181,
    'date-added': 181,
    'date-last-opened': 181,
    'size': 97,
    'kind': 115,
    'label': 100,
    'version': 75,
    'comments': 300,
}

list_column_sort_directions = {
    'name': 'ascending',
    'date-modified': 'descending',
    'date-created': 'descending',
    'date-added': 'descending',
    'date-last-opened': 'descending',
    'size': 'descending',
    'kind': 'ascending',
    'label': 'ascending',
    'version': 'ascending',
    'comments': 'ascending',
}
