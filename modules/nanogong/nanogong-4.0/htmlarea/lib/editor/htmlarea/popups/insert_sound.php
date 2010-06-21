<?php // $Id: insert_sound.php,v 4 2010/04/22 00:00:00 gibson Exp $

    require("../../../../config.php");

    $id = optional_param('id', SITEID, PARAM_INT);

    require_login($id);

    @header('Content-Type: text/html; charset=utf-8');
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<title><?php print_string("insertsound","editor");?></title>
<script type="text/javascript" src="popup.js"></script>
<script type="text/javascript">
//<![CDATA[

function Init() {
    __dlg_init();
    var param = window.dialogArguments;
    if (param) {
        document.getElementById("f_caption").value = param["f_caption"];
        document.getElementById("f_sndurl").value = param["f_sndurl"];
    }
    document.getElementById("f_caption").focus();
    if (param && param["f_sndurl"] && param["f_sndurl"] != "") setTimeout("loadSound()", 1000);
};

var checkAppletCount = 0

function loadSound() {
    var recorder = document.getElementById("recorder");
    if (recorder == null) return;

    var url = "";
    var version = recorder.sendGongRequest("GetVersion", "");
<?php
    if ($CFG->slasharguments)
        $url = "{$CFG->wwwroot}/file.php";
    else
        $url = "{$CFG->wwwroot}/file.php?file=";
?>
    if (version != "") url = recorder.sendGongRequest("LoadFromURL", "<?php p($url) ?>" + document.getElementById("f_sndurl").value);

    if (url == null || url == "") {
        checkAppletCount++;
        if (checkAppletCount < 10)
            setTimeout("loadSound()", 1000);
        else
            alert("<?php print_string("loadfailure", "editor");?>");
    }
}

function onOK() {
    var recorder = document.getElementById("recorder");
    if (recorder == null) {
        alert("<?php print_string("recordernotready", "editor");?>");
        return false;
    }

    // check whether the voice is ready
    var duration = recorder.sendGongRequest("GetMediaDuration", "audio");
    if (duration == null || duration == "" ||
        isNaN(duration) || parseInt(duration) <= 0) {
        alert("<?php print_string("norecording", "editor");?>");
        return false;
    }

    // check whether the applet is modified
    var modified = recorder.getModified();
    if (modified == null || modified != "1") {
        if (document.getElementById("f_sndurl").value == null ||
            document.getElementById("f_sndurl").value == "") return false;
    }
    else {
        // upload the voice file to the server
        var url = recorder.sendGongRequest("PostToForm", "../uploadsound.php?sesskey=<?php p($USER->sesskey) ?>", "userfile", "", "temp");
        if (url == null || url == "") {
            alert("<?php print_string("uploadfailure", "editor");?>");
            return false;
        }
        document.getElementById("f_sndurl").value = url;
    }

    // pass data back to the calling window
    var fields = ["f_caption", "f_sndurl"];
    var param = new Object();
    for (var i in fields) {
        var id = fields[i];
        var el = document.getElementById(id);
        param[id] = el.value;
    }
    __dlg_close(param);
    return false;
};

function onCancel() {
    __dlg_close(null);
    return false;
};

//]]>
</script>
<style type="text/css">
html, body {
margin: 2px;
background-color: rgb(212,208,200);
font-family: Tahoma, Verdana, sans-serif;
font-size: 11px;
}
.title {
background-color: #ddddff;
padding: 5px;
border-bottom: 1px solid black;
font-family: Tahoma, sans-serif;
font-weight: bold;
font-size: 14px;
color: black;
}
td, input, select, button {
font-family: Tahoma, Verdana, sans-serif;
font-size: 11px;
}
button { width: 70px; }
.space { padding: 2px; }
form { margin-bottom: 0px; margin-top: 0px; }
</style>
</head>
<body onload="Init()">
  <div class="title"><?php print_string("insertsound","editor");?></div>
  <div class="space"></div>
  <div class="space"></div>
  <div class="space"></div>
  <form action="" method="get" id="first" onsubmit="return false">
    <div style="text-align: center">
      <center>
      <fieldset style="width: 190px">
      <legend><?php print_string("soundrecorder","editor");?></legend>
      <div class="space"></div>
      <div class="space"></div>
      <div style="text-align: center">
      <applet archive="../nanogong.jar" code="gong.NanoGong" id="recorder" name="recorder" width="180" height="40">
        <param name="ShowTime" value="true" />
      </applet>
      </div>
      </fieldset>
      </center>
    </div>
    <br />
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="20%"><?php print_string("caption","editor");?>:</td>
        <td width="80%"><input name="f_caption" type="text" id="f_caption" style="width: 100%;" /></td>
      </tr>
    </table>
    <br />
    <table align="center" width="80%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td align="center">
          <button name="btnOK" type="button" id="btnOK" onclick="return onOK();"><?php print_string("ok","editor") ?></button></td>
        <td align="center">
          <button name="btnCancel" type="button" id="btnCancel" onclick="return onCancel();"><?php print_string("cancel","editor") ?></button></td>
      </tr>
    </table>
    <div class="space"></div>
    <input name="f_sndurl" type="hidden" id="f_sndurl" />
    <input name="duration" type="hidden" id="duration" />
  </form>
  <p>&nbsp;</p>
</body>
</html>
