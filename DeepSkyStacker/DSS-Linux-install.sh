
#!/bin/bash
#
# Register mime type for dssfilelist files
#
sudo xdg-mime install text-dssfilelist.xml
#
# And an icon for the same
#
sudo xdg-icon-resource install --context mimetypes --size 64 DSS_filelist64x64.png text-dssfilelist
sudo update-mime-database /usr/share/mime

#
# install desktop files to the default location
#
sudo desktop-file-install /opt/DeepSkyStacker/DeepSkyStacker.desktop
sudo desktop-file-install /opt/DeepSkyStacker/DeepSkyStackerLive.desktop
sudo update-desktop-database /usr/share/applications



