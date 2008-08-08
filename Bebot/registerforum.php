<?php
///////////////////////////////////
// forumregister.php 1.0 for BeBot
///////////////////////////////////
// (c) Copyright 2008 by Hannes Nagl
// All Rights Reserved
// Licensed for distribution under the GPL (Gnu General Public License) version 2.0 or later
// Parts of the code copied from Allan Noer
///////////////////////////////////

$forumregister = new forumregister($bot);

//////////////////////////////////////////////////////////////////////
// The Class itself...
class forumregister Extends BaseActiveModule
{
	var $bot;
	var $help;

	var $last_log;
	var $start;

	// Constructor
	function __construct (&$bot)
	{
		parent::__construct(&$bot, get_class($this));

		$this -> last_log = array();
		$this -> start = time() + 3600;

		$this -> output = "group";
		$this -> result = "";

		$this -> register_event("buddy");
		$this -> register_event("connect");
		$this -> help['description'] = 'Reminds users to register on the forums.';

		$this -> bot -> core("settings") -> create("Forumregister", "Remind", TRUE, "Should users be reminded to register on the forums?");
	}

	function buddy($name, $online, $level, $location)
	{
		if(0 == $online || 1 == $online)
		{
			if ($this -> bot -> core("notify") -> check($name) && $this -> bot -> core("settings") -> get("Forumregister", "Remind"))
			{
			//	$id = $this -> bot -> core("chat") -> get_uid($name);
				if ($online == 1)
				{
					if ($this -> last_log["on"][$name] < (time() - 5))
					{
						// Select a list of all users currently known by the forums
						// this query also selects the alts to make sure you're not bugged all the time
						$result = $this -> bot -> db -> select('SELECT v.Value AS name 
						FROM drupal.users u 
						JOIN drupal.profile_values v 
						ON u.uid = v.uid 
						WHERE v.fid = 1 
						AND u.uid != "0" 
						UNION SELECT a.alt AS name 
						FROM bebot.alts a 
						WHERE a.main IN (
						SELECT v.Value 
						FROM drupal.users u 
						JOIN drupal.profile_values v 
						ON u.uid = v.uid 
						WHERE v.fid = 1 AND u.uid != "0");', MYSQL_ASSOC);
						$registered = false;
						foreach ($result as $value)
							{	
								if(strtolower($value['name']) == strtolower($name))
								{
									$registered = true;
								}
							}
						if(!$registered)
						{
							$msg = "You are currently not registered in our forums! Please go to http://example.com/ to stop these messages. If this is an Alt please whisper from your mainchar  '/tell BOT !alts add <altname>'.";
							$this -> bot -> send_tell($name, $msg);
						}
						$this -> last_log["on"][$name] = time();
					}
				}
				else
				{
					if ($this -> last_log["off"][$name] < (time() - 5))
					{
						$this -> last_log["off"][$name] = time();
					}
				}
			}
		}
	}

	function command_handler($name, $msg, $origin)
	        {
			$output = "";
			return $output;
		}

	function connect()
	{
		$this -> start = time() + 3 * $this -> bot -> crondelay;
	}

}

?>
