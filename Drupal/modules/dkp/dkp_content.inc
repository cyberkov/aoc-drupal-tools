<?php
/*******************************************************
 *                                                     *
 *                 List functions                      *
 *                                                     *
*******************************************************/

function dkp_members_list() {
		$header = _dkp_build_header( array(
				array('data' => t('Name'), 'field' => 'm.member_name'),
				array('data' => t('Klasse'), 'field' => 'c.class_name'),
				array('data' => t('EP'), 'field' => 'm.member_earned'),
				array('data' => t('GP'), 'field' => 'm.member_spent'),
				array('data' => t('Korrektur'), 'field' => 'm.member_adjustment'),
				array('data' => t('Anrecht'), 'sort' => 'desc', 'field' => 'member_current'),
				array('data' => t('Letzter Raid'), 'field' => 'm.member_lastraid'),		
				) );
		$sql = 'SELECT m.* , c.* , ((m.member_earned + m.member_adjustment) / m.member_spent) AS member_current
				FROM eqdkp_members m, eqdkp_classes c
				WHERE c.class_id = m.member_class_id AND m.member_lastraid > 0 ' . tablesort_sql($header);
		$sql_count = 'SELECT COUNT(m.member_name) FROM eqdkp_members m WHERE m.member_lastraid > 0';
		$result = pager_query($sql, 25);
		
		while ($members = db_fetch_object($result)) {
		  $rows[] = _dkp_build_content( $members->member_name, 'members', array(
			_dkp_link_member($members),
			_dkp_link_text($members->class_name, 'class'),
			_dkp_link_value($members->member_earned),
			_dkp_link_value($members->member_spent),
			_dkp_link_value($members->member_adjustment),
			_dkp_link_value($members->member_current),
			_dkp_link_date($members->member_lastraid),
			));
		}
		$output .= theme('table', $header, $rows);
		if ($pager = theme('pager', NULL, 25, 0)) {
		  $output .= $pager;
		}
	return $output;
}

function dkp_raids_list() {	
	$sql = 'SELECT raid_id, raid_name, raid_date, raid_note, raid_value 
        FROM eqdkp_raids
        ORDER BY raid_date desc';
		
	$sql_count = 'SELECT COUNT(r.raid_id) FROM eqdkp_raids r';
	$result = pager_query($sql, 25, 0, $sql_count);
  	while ($raids = db_fetch_object($result)) {
		$rows[] = _dkp_build_content( $raids->raid_id, 'raids', array(
			  _dkp_link_date($raids->raid_date),			  _dkp_link_raid($raids),
			  _dkp_link_text($raids->raid_note, 'note'),
			  _dkp_link_value($raids->raid_value),
			));
  	}
	$header = _dkp_build_header( array(t('Date'), t('Name'), t('Note'), t('Value')) );
	$output .= theme('table', $header, $rows);
	if ($pager = theme('pager', NULL, 25, 0)) {
      		$output .= $pager;
      	}
    return $output;
}

function dkp_items_list() {
	$sql = 'SELECT i.*, r.raid_name, r.raid_id, m.member_id, m.member_name, c.class_name
			FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m, eqdkp_classes c
			WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND m.member_class_id = c.class_id';

	$sql_count = 'SELECT COUNT(i.item_id) FROM eqdkp_items i';
			
	$result = pager_query($sql, 25, 0, $sql_count);
	
  	while ($loot = db_fetch_object($result)) {
		$rows[] = _dkp_build_content( $loot->item_id, 'items', array(
			  _dkp_link_date($loot->item_date),
			  _dkp_link_member($loot),
			  _dkp_link_item($loot),
			  _dkp_link_raid($loot),
			  _dkp_link_value($loot->item_value)
			));
  	}
	$header = _dkp_build_header( array(t('Date'), t('Buyer'), t('Item'), t('Raid'), t('Value')) );
	$output .= theme('table', $header, $rows);
	if ($pager = theme('pager', NULL, 25, 0)) {
      $output .= $pager;
      }
return $output;
}	

function dkp_events_list() {
	$sql = 'SELECT event_id, event_name, event_value
        FROM eqdkp_events
        ORDER BY event_name desc';
		
	$sql_count = 'SELECT COUNT(r.event_id) FROM eqdkp_events r';
			
	$result = pager_query($sql, 25, 0, $sql_count);
	
  	while ($events = db_fetch_object($result)) {
		$rows[] = _dkp_build_content( $events->event_id, 'events', array(
		           _dkp_link_event($events),
	  		   _dkp_link_value($events->event_value)
		));
  	}
	
	$header = _dkp_build_header( array(t('Name'), t('Value')) );
	$output .= theme('table', $header, $rows);
	if ($pager = theme('pager', NULL, 25, 0)) {
		$output .= $pager;
	  }
  	return $output;
}

/*******************************************************
 *                                                     *
 *                 View functions                      *
 *                                                     *
*******************************************************/

function dkp_members_view($uid = 0) {
	// Summary Information
		$sql = 'SELECT m.*, ((m.member_earned + m.member_adjustment) / m.member_spent) as member_dkp
			FROM eqdkp_members m WHERE m.member_id = ' . $uid;
		$result = db_query($sql);
		$member = db_fetch_object($result);
		$memberRows[] = _dkp_build_content( $member->member_name, 'members', array(
			 _dkp_link_value($member->member_earned),
			 _dkp_link_value($member->member_spent),
			 _dkp_link_value($member->member_adjustment),
			 _dkp_link_value($member->member_dkp),
			));

		$header = _dkp_build_header( array(t('EP'), t('GP'), t('Korrektur'), t('Anrecht')));
		$output .= theme('table', $header, $memberRows, '', $member->member_name);
	
		// Raid Attendence History
		$sql = 'SELECT r.*
			FROM eqdkp_raids r, eqdkp_raid_attendees a, eqdkp_members m
			WHERE r.raid_id = a.raid_id AND a.member_name = m.member_name AND m.member_id = ' . $uid . '
			ORDER BY raid_date desc';
			
		$sql_count = 'SELECT COUNT(r.raid_id) 
						FROM eqdkp_raids r, eqdkp_raid_attendees a, eqdkp_members m
						WHERE r.raid_id = a.raid_id AND a.member_name = m.member_name AND m.member_id = ' . $uid;
				
		$result = pager_query($sql, 25, 0, $sql_count);
		
		while ($raids = db_fetch_object($result)) {
			 $raidRows[] = array(
			 _dkp_link_date($raids->raid_date),
			 _dkp_link_raid($raids),
			 _dkp_link_text($raids->raid_note, 'note'),
			 _dkp_link_value($raids->raid_value),
			 );
		}
		
		$header = array(t('Date'), t('Name'), t('Note'), t('Value'));
		$output .= theme('table', $header, $raidRows, '', t('Raid Attendance History'));
		if ($pager = theme('pager', NULL, 25, 0)) {
		  $output .= $pager;
		}
	
		// List the member's loot
		$rows[] = array();
//		drupal_add_css( drupal_get_path('module', 'warcraft_armory') . '/templates/master.css','module','all',false);
//		drupal_add_js(  variable_get('warcraft_armory_overlib_path', drupal_get_path('module', 'warcraft_armory') . '/overlib/overlib.js'));
		
		$sql = 'SELECT i.*, m.member_id, r.raid_name 
				FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m 
				WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND m.member_id = ' . $uid;
			
		$sql_count = 'SELECT COUNT(i.item_id)
						FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m 
						WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND m.member_id = ' . $uid;
				
		$result = pager_query($sql, 25, 0, $sql_count);
		
		while ($loot = db_fetch_object($result)) {
			 $lootRows[] = array(
			 _dkp_link_date($loot->item_date),
			 _dkp_link_item($loot),
			 _dkp_link_raid($loot),
			 _dkp_link_value($loot->item_value),
			 );
		}
		
		$header = array(t('Date'), t('Item'), t('Raid'), t('Value'));
		$output .= theme('table', $header, $lootRows, '' , t('Item Purchase History'));
		if ($pager = theme('pager', NULL, 25, 1)) {
		  $output .= $pager;
		}
		
		
		// Individual Adjustment History	
		$sql = 'SELECT a.* FROM eqdkp_adjustments a, eqdkp_members m WHERE a.member_name = m.member_name and m.member_id =  ' . $uid . '
			ORDER BY a.adjustment_date desc';
			
		$sql_count = 'SELECT COUNT(a.adjustment_id) 
						FROM eqdkp_adjustments a, eqdkp_members m WHERE a.member_name = m.member_name and m.member_id = ' . $uid;
				
		$result = pager_query($sql, 25, 0, $sql_count);
		
		while ($adjustments = db_fetch_object($result)) {
			 $adjustmentRows[] = array(
				 _dkp_link_date($adjustments->adjustment_date),
				 _dkp_link_text($adjustments->adjustment_reason, 'reason'),
				 _dkp_link_value($adjustments->adjustment_value),
			  );
		}
		$header = array(t('Date'), t('Reason'), t('Adjustment'));
		$output .= theme('table', $header, $adjustmentRows, '', t('Individual Adjustment History'));
		if ($pager = theme('pager', NULL, 25, 2)) {
		  $output .= $pager;
		}
		
		
		// Attendance by Event
		$sql = 'SELECT r.*, e.event_id, e.event_name
        		FROM eqdkp_raids r, eqdkp_events e
			WHERE e.event_name = r.raid_name
				GROUP BY r.raid_name
        		ORDER BY r.raid_name desc';													
		$sql_count = 'SELECT COUNT(r.raid_id)
        		FROM eqdkp_raids r
				GROUP BY r.raid_name';
		$result = pager_query($sql, 25, 0, $sql_count);					// Get a list of raids	
		
		while ($raids = db_fetch_object($result)) {
			
			$sql_count_raids = 'SELECT COUNT(r.raid_id)
        		FROM eqdkp_raids r
				WHERE r.raid_name = \'' . $raids->raid_name . '\'';
				
			$sql_count_attendance = 'SELECT COUNT(a.raid_id)
        		FROM eqdkp_raid_attendees a, eqdkp_members m
				WHERE a.raid_id = ' . $raids->raid_id . '
				AND a.member_name = m.member_name
				AND m.member_id = ' . $uid;
								
			$count_attendance = db_result(db_query($sql_count_raids));
			$count_raids = db_result(db_query($sql_count_attendance));
			$raidPercent = ( $count_attendance > 0 ? ($count_raids/$count_attendance)*100 : 0);
		
			$eventRows[] = array(
			_dkp_link_event($raids),
			_dkp_link_text($count_raids, 'count'),
			_dkp_link_text(number_format($raidPercent,0).'%', 'count'),
			_dkp_link_percent_bar($raidPercent),
			);
		}
		
		$header = array(t('Event'), t('#'), t('%'), t(''));
		$output .= theme('table', $header, $eventRows, '', t('Attendance by Event'));
		if ($pager = theme('pager', NULL, 25, 3)) {
		  	$output .= $pager;
		  }
return $output;
}

function dkp_raids_view($raid_id = 0) {
		// Summary Information
		$sql = 'SELECT r.*, e.event_id, e.event_name
			FROM eqdkp_raids r, eqdkp_events e
			WHERE r.raid_name = e.event_name AND r.raid_id = ' . $raid_id;
		$result = db_query($sql);
		$raid = db_fetch_object($result);
		
		$raidRows[] = array(
			  _dkp_link_date($raid->raid_date),
			  _dkp_link_text($raid->raid_note, 'note'),
			  _dkp_link_event($raid),
			  _dkp_link_value($raid->raid_value),
			  _dkp_link_text($raid->raid_added_by, 'admin'),
			  _dkp_link_text($raid->raid_updated_by, 'admin'),
			);
		
		$header = array(t('Date'), t('Note'), t('Event'), t('Value'), t('Added by'), t('Updated by'));
		$output .= theme('table', $header, $raidRows, '', t($raid->raid_name));

		// Attendees
		$sql = 'SELECT m.member_id, m.member_name, c.class_name
			FROM eqdkp_raid_attendees a, eqdkp_members m, eqdkp_classes c
			WHERE c.class_id = m.member_class_id AND a.member_name = m.member_name AND a.raid_id = '. $raid_id .'
			ORDER BY m.member_name';
		$result = db_query($sql);

		//Setting data for the Class distribution part		
		$wowclasses_count = array();
		$wowclasses_total=0;
		//optionnal : used to sort the classes 
		$class_sql = 'SELECT class_name FROM eqdkp_classes LIMIT 1, 5000';
		$class_result = db_query($class_sql);
		$wowclasses = array();
		while ($class_field = db_fetch_object($class_result))
		{
			$wowclasses[$class_field->class_name] = '';
		}

		$mem_per_line = 5;
		$i=0; $member_group=array();
		while ($member = db_fetch_object($result)) {
			if ($i < $mem_per_line) { 
				$member_group[] =  _dkp_link_member($member);
				$i++;
			} else {
				$memberRows[] = $member_group;
				$member_group=array();
				$i=0;
			}

			//Grab data for the Class distribution part
			if ($wowclasses_count[$member->class_name]>0) 
			{
				$wowclasses[$member->class_name] .= ", ";
			}

			$wowclasses[$member->class_name] .= l($member->member_name,'dkp/'.$member->member_id);
			$wowclasses_count[$member->class_name]++;
			$wowclasses_total++;
			}

		$header = array(t(''), t(''), t(''), t(''), t(''));
		$output .= theme('table', $header, $memberRows, '', t('Attendees'));
	
		// List loots
		$rows[] = array();
		$sql = 'SELECT i.*, m.member_id, m.member_name, r.raid_name
				FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m
				WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND r.raid_id = ' . $raid_id .'
				ORDER BY r.raid_date, i.item_name';
			
		$sql_count = 'SELECT COUNT(i.item_id)
				FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m
				WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND r.raid_id = ' . $raid_id;
		$result = pager_query($sql, 25, 0, $sql_count);

		while ($loot = db_fetch_object($result)) {
			 $lootRows[] = array(
			  _dkp_link_member($loot),
			  _dkp_link_item($loot),
			  _dkp_link_value($loot->item_value),
			  );
		}
		$header = array(t('Buyer'), t('Item'), t('Value'));
		$output .= theme('table', $header, $lootRows, '' , t('Drops'));
		if ($pager = theme('pager', NULL, 25, 1)) {
		  $output .= $pager;
		}
	
		// Class Distribution
		foreach ($wowclasses as $wowcl => $text) {
			 $percent= $wowclasses_count[$wowcl] / $wowclasses_total * 100;
			 $classesRows[] = array(			  _dkp_link_text($wowcl, 'class'),
			  _dkp_link_text($wowclasses_count[$wowcl], 'count'),
			  _dkp_link_text(round($percent).'%', 'count'),
			  _dkp_link_percent_bar($percent),
			  _dkp_link_text($text, $wowcl),
			  );
		}

		$header = array(t('Class'), t('#'), t('%'), t(''), t('Attendees'));
		$output .= theme('table', $header, $classesRows, '', t('Class Distribution'));
	
    	return $output;
}

function dkp_items_view($loot_id = 0) {
		// List items matching
		$sql = 'SELECT i.*, r.raid_name, m.member_name, m.member_id, c.class_name
			FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m, eqdkp_classes c
			WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND m.member_class_id = c.class_id AND i.item_id = ' . $loot_id;
		$result = db_query($sql);
		$item = db_fetch_object($result);
		
		$itemRows[] = array(
			  _dkp_link_date($item->item_date),
			  _dkp_link_member($item),
			  _dkp_link_raid($item),
			  _dkp_link_value($item->item_value),
			);

		$header = array(t('Date'), t('Buyer'), t('Raid'), t('Value'));
		$output .= theme('table', $header, $itemRows, '', $item->item_name);	return $output;			
}


function dkp_events_view($event_id = 0) {
		// Summary Information
		$sql = 'SELECT m.* FROM eqdkp_events m WHERE m.event_id = ' . $event_id;
		$result = db_query($sql);
		$event = db_fetch_object($result);
		$eventRows[] = array(
			_dkp_link_value($event->event_value),
			_dkp_link_text($event->event_added_by, 'admin'),
			_dkp_link_text($event->event_updated_by, 'admin'),
			);
		$header = array(t('Event value'), t('Added by'), t('Updated by'));
		$output .= theme('table', $header, $eventRows, '', t($event->event_name));
	
		// Raid Attendence History
		$sql = 'SELECT DISTINCT r.raid_id, r.*, COUNT(m.member_name) as count_members
			FROM eqdkp_raids r, eqdkp_raid_attendees a, eqdkp_members m, eqdkp_events e, eqdkp_items i
			WHERE r.raid_id = a.raid_id AND a.member_name = m.member_name AND r.raid_name = e.event_name 
				AND i.raid_id = r.raid_id AND e.event_id = '. $event_id .'
			GROUP BY r.raid_id
			ORDER BY raid_date desc';
			
		$sql_count = 'SELECT COUNT(r.raid_id) 
				FROM eqdkp_raids r, eqdkp_raid_attendees a, eqdkp_members m, eqdkp_events e
				WHERE r.raid_id = a.raid_id AND a.member_name = m.member_name AND r.raid_name = e.event_name AND e.event_id = '. $event_id;
				
		$result = pager_query($sql, 25, 0, $sql_count);
		
		while ($raids = db_fetch_object($result)) {
		       $raidRows[] = array(
			  _dkp_link_date($raids->raid_date),
			  _dkp_link_text($raids->count_members, 'count'),
			  _dkp_link_text('', 'items'),
			  _dkp_link_text($raids->raid_note, 'note'),
			  _dkp_link_value($raids->raid_value),
			  );
		}
		
		$header = array(t('Date'), t('Attendees'), t('Drops'), t('Note'), t('Value'));
		$output .= theme('table', $header, $raidRows, '', t('Raid Attendance History'));
		if ($pager = theme('pager', NULL, 25, 0)) {
		  $output .= $pager;
		}

		// List event loots
		$rows[] = array();
//		drupal_add_css( drupal_get_path('module', 'warcraft_armory') . '/templates/master.css','module','all',false);
//		drupal_add_js(  variable_get('warcraft_armory_overlib_path', drupal_get_path('module', 'warcraft_armory') . '/overlib/overlib.js'));
		
		$sql = 'SELECT i.*, m.member_id, m.member_name, r.raid_name, e.event_id, c.class_name
				FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m, eqdkp_events e, eqdkp_classes c
				WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND r.raid_name = e.event_name 
					AND c.class_id = m.member_class_id  AND e.event_id = ' . $event_id .'
				ORDER BY r.raid_date, i.item_name';
			
		$sql_count = 'SELECT COUNT(i.item_id)
				FROM eqdkp_items i, eqdkp_raids r, eqdkp_members m, eqdkp_events e
				WHERE r.raid_id = i.raid_id AND i.item_buyer = m.member_name AND r.raid_name = e.event_name AND e.event_id = ' . $event_id;
				
		$result = pager_query($sql, 25, 0, $sql_count);
		
		while ($loot = db_fetch_object($result)) {
			 $lootRows[] = array(
			  _dkp_link_date($loot->item_date),
			  _dkp_link_member($loot),
			  _dkp_link_item($loot),
			  _dkp_link_value($loot->item_value),
			  );
		}
		
		$header = array(t('Date'), t('Buyer'), t('Item'), t('Value'));
		$output .= theme('table', $header, $lootRows, '' , t('Item Purchase History'));
		if ($pager = theme('pager', NULL, 25, 1)) {
		  $output .= $pager;
		}
    	return $output;
}

/*******************************************************
 *                                                     *
 *                        A P I                        *
 *                                                     *
*******************************************************/

function _dkp_build_header($h) {
	$head = array();
	$admin= _dkp_link_edit_header();
	if (is_array($admin)) $head[]=$admin;
	foreach ($h as $item) {
	  $head[]=$item;
	} 
	return $head;
}

function _dkp_build_content($row, $type, $f) {
	$ret = array();
	$admin= _dkp_link_edit($row, $type);
	if (is_array($admin)) $ret[]=$admin;
	foreach ($f as $item) {
	  $ret[]=$item;
	} 
	return $ret;
}



function _dkp_link_member($member) {
return array('data' => l($member->member_name,'dkp/members/'.$member->member_id), 'class' => 'member '. _dkp_link_member_color($member->class_name));
}

function _dkp_link_raid($raid) {
return array('data' => l($raid->raid_name, 'dkp/raids/'. $raid->raid_id), 'class' => 'raid');
}

function _dkp_link_item($item) {
//EDIT : Choose the item linking function you prefer (or none)
$link_method=0;
switch ($link_method) {
	case 1: $data = _warcraft_armory_process('[item]'.$item->item_name.'[/item]'); break;
	case 2: $data = _lootz_link_item($item->item_name); break;
	default: $data = l($item->item_name, 'dkp/items/'.$item->item_id);
	}
return array('data' => $data, 'class' => 'item');
}

function _dkp_link_event($event) {
return array('data' => l($event->event_name,'dkp/events/'.$event->event_id), 'class' => 'event');	
}

function _dkp_link_edit($name, $type) {
//EDIT : Choose if you want to get links to your edit pages  (soon as access rights)
//EDIT : And set data to get the good links to your eqdkp admin pages
$admin=0;
$eqdkp_install='http://127.0.0.1/dkpdrupal/admin/';
$eqdkp_admin=array(
	'members' => 'manage_members.php?s=&mode=addmember&name=',
	'raids' => 'addraid.php?s=&r=',
	'items' => 'additem.php?s=&i=',
	'events' => 'addevent.php?s=&e=',
	);
//$edit_text = 'edit<img src="/modules/dkp_5.x-0.1-dev/edit.gif">';
$edit_text = 'edit';
if ($admin == 1) return array('data' => '<a href="'. $eqdkp_install . $eqdkp_admin[$type] . $name .'">'. $edit_text .'</a>', 'class' => 'edit');
  else return;
}

function _dkp_link_edit_header() {
//EDIT : Choose if you want to get links to your edit pages  (soon as access rights)
$admin=0;
if ($admin == 1) return array('data' => '');
  else return;
}

function _dkp_link_text($text, $class) {
return array('data' => $text, 'class' => $class);
}

function _dkp_link_date($date) {
//EDIT : Choose your date format
$date_method=1;
switch ($date_method) {
	case '1': $format='d/m/Y'; break;
	default: $format='m/d/Y'; break;
	}
//   ($members->member_lastraid > 0) ? format_date($members->member_lastraid,'custom','m/d/Y') : 'Never'; 
return array('data' => format_date($date,'custom',$format), 'class' => 'date');
}
			
function _dkp_link_value($value) {
$value_method=1;
switch ($value_method) {
	case 1: $ret = round($value); break;
	default: $ret = $value; break;
	}
return array('data' => $ret, 'class' => _dkp_link_value_color($value));
}

function _dkp_link_percent_bar($percent) {
return _dkp_link_text('<div style="width:'. 2*$percent.'px;" class="dkp_percent_bar"></div>', 'dkp_percent');
}

function _dkp_link_member_color($class) {
//EDIT : Choose if you want to use Class Color
$member_method=1;
switch ($member_method) {
	case 1: return $class;
	default: return;
	}
}

function _dkp_link_value_color($value) {
if ($value > 0) return 'positive';
if ($value < 0) return 'negative';
return 'neutral';
}


/*
function dkp_user($type, &$edit, &$user, $category = NULL) {
  if ($type == 'view') {
    $items['DKP'] = array('title' => t('DKP'),
      'value' => 'test',
      'class' => 'member',
    );

    return array(t('DKP') => $items);
  }
}
*/

?>
