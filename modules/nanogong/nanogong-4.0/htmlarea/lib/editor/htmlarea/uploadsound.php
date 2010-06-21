<?php // $Id: uploadsound.php,v 3.0 2008/08/13 00:00:00 gibson Exp $

//  Handle uploading of sound

    require("../../../config.php");
    require_once($CFG->libdir.'/filelib.php');
    require_once($CFG->dirroot.'/lib/uploadlib.php');

    require_login();

    if (! $basedir = make_upload_directory(SITEID)) {
        error("The site administrator needs to fix the file permissions");
    }

    $baseweb = $CFG->wwwroot;

    if (confirm_sesskey()) {
        // remove the annoying warning from upload manager
        $el = error_reporting(0);

        // hardcode the file name
        if (isset($_FILES['userfile']['name'])) {
            $oldname = $_FILES['userfile']['name'];
            $ext = preg_replace("/.*(\.[^\.]*)$/", "$1", $oldname);
            $newname = date("Y-m-d") . "_" . date("His") . $ext;
            $_FILES['userfile']['name'] = $newname;
        }
        
        // handle the upload
        $um = new upload_manager('userfile',false,true,null,false,0,true);
        $wdir = "/nanogong_files/{$USER->id}";
        $dir = "$basedir$wdir";
        if ($um->process_file_uploads($dir)) {
            $filename = $um->get_new_filename();
            $fileurl = "$wdir/" . $filename;
            print "/".SITEID."$fileurl";
        }
        error_reporting($el);
    }

?>
