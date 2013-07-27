This is a modified "WME Demo" project with touch-friendly controls. It can serve
as an inspiration on how to implement touch interface for your game.


Controls:

- touch an active spot to display the action menu
- slide around to display object captions (release finger on an active spot to display
the action menu)
- touch the cog icon to display main menu
- touch the box icon to display/close the inventory
- touch an inventory item to make it active (when an item is active, touching other items
or other active spots uses the selected item on them)
- touch the selected item icon to deselect
- hold your finger on an inventory item to display its action menu


Implementation notes

- Unlike PC games, most processing is done in the LeftRelease event.
- To select an inventory item, don't use the usual Game.SelectedItem = [something];
but instead call Game.SelectItem([something]);
- The game uses the DejaVu font of size 20. That seems to be a reasonable size for
outlined fonts for non-retina iPhone displays. Retina displays and iPads would probably be
able to use smaller fonts. Remember that used TrueType fonts must be distributed with
the game to work on iOS.
