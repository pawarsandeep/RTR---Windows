<?php
namespace Drupal\shm_d3_visualization\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;

class CustomRestExports extends ControllerBase {
	public function getServiceEngineerJobs() {
		$start = date('Y-m-d');
		$end = date('2016-11-01');
	  
		$userId = Drupal::currentUser()->id();
		$res = db_query("SELECT substring(eform_submission__field_date_of_inspection.field_date_of_inspection_value, 6,2) as month, COUNT(eform_submission_field_data.eid) AS count 
		FROM 
		{eform_submission_field_data} eform_submission_field_data
		LEFT JOIN {eform_submission__field_date_of_inspection} eform_submission__field_date_of_inspection ON eform_submission_field_data.eid = eform_submission__field_date_of_inspection.entity_id AND (eform_submission__field_date_of_inspection.deleted = '0' AND eform_submission__field_date_of_inspection.langcode = eform_submission_field_data.langcode)
		LEFT JOIN {eform_submission__field_service_engineer} eform_submission__field_service_engineer ON eform_submission_field_data.eid = eform_submission__field_service_engineer.entity_id AND (eform_submission__field_service_engineer.deleted = '0' AND eform_submission__field_service_engineer.langcode = eform_submission_field_data.langcode)
		WHERE (( (eform_submission_field_data.type IN ('certificate')) AND
		(eform_submission__field_date_of_inspection.field_date_of_inspection_value BETWEEN '$end' AND '$start') AND
		(eform_submission__field_service_engineer.field_service_engineer_target_id=$userId) AND (eform_submission_field_data.moderation_state LIKE 'approved')))
		GROUP BY substring(eform_submission__field_date_of_inspection.field_date_of_inspection_value, 6,2)")->fetchAll();
		
		return new JsonResponse($res);
	}

	public function getJobsByCountry() {
		$india = db_query("select 'India' as Country,
		sum(approved) as Approved, sum(draft) as Draft, sum(in_progress) as 'In Progress',sum(pending_approval) as 'Pending Approval',sum(rejected) as Rejected
		from (
		select eform_submission__field_vessel_flag_field_vessel_flag_value,
		case when eform_submission_field_data_moderation_state = 'approved' then eform_submission_field_data_created end as Approved,
		case when eform_submission_field_data_moderation_state = 'draft' then eform_submission_field_data_created end as draft,
		case when eform_submission_field_data_moderation_state = 'in_progress' then eform_submission_field_data_created end as in_progress,
		case when eform_submission_field_data_moderation_state = 'pending_approval' then eform_submission_field_data_created end as pending_approval,
		case when eform_submission_field_data_moderation_state = 'rejected' then eform_submission_field_data_created end as rejected
		from (
		SELECT eform_submission_field_data.moderation_state AS eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag.field_vessel_flag_value AS eform_submission__field_vessel_flag_field_vessel_flag_value, COUNT(eform_submission_field_data.created) AS eform_submission_field_data_created, MIN(eform_submission_field_data.eid) AS eid
		FROM
		eform_submission_field_data eform_submission_field_data
		LEFT JOIN eform_submission__field_vessel_flag eform_submission__field_vessel_flag ON eform_submission_field_data.eid = eform_submission__field_vessel_flag.entity_id AND (eform_submission__field_vessel_flag.deleted = '0' AND eform_submission__field_vessel_flag.langcode = eform_submission_field_data.langcode)
		WHERE (( (eform_submission_field_data.type IN  ('', '', '', 'certificate')) AND eform_submission__field_vessel_flag.field_vessel_flag_value = 'IN' ))
		GROUP BY eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag_field_vessel_flag_value
		ORDER BY eform_submission_field_data_moderation_state ASC
		LIMIT 10 OFFSET 0) t) t1")->fetchAll();
		$japanese = db_query("select 'Japenese' as Country,
			sum(approved) as Approved, sum(draft) as Draft, sum(in_progress) as 'In Progress',sum(pending_approval) as 'Pending Approval',sum(rejected) as Rejected
			from (
			select eform_submission__field_vessel_flag_field_vessel_flag_value,
			case when eform_submission_field_data_moderation_state = 'approved' then eform_submission_field_data_created end as Approved,
			case when eform_submission_field_data_moderation_state = 'draft' then eform_submission_field_data_created end as draft,
			case when eform_submission_field_data_moderation_state = 'in_progress' then eform_submission_field_data_created end as in_progress,
			case when eform_submission_field_data_moderation_state = 'pending_approval' then eform_submission_field_data_created end as pending_approval,
			case when eform_submission_field_data_moderation_state = 'rejected' then eform_submission_field_data_created end as rejected
			from (
			SELECT eform_submission_field_data.moderation_state AS eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag.field_vessel_flag_value AS eform_submission__field_vessel_flag_field_vessel_flag_value, COUNT(eform_submission_field_data.created) AS eform_submission_field_data_created, MIN(eform_submission_field_data.eid) AS eid
			FROM
			eform_submission_field_data eform_submission_field_data
			LEFT JOIN eform_submission__field_vessel_flag eform_submission__field_vessel_flag ON eform_submission_field_data.eid = eform_submission__field_vessel_flag.entity_id AND (eform_submission__field_vessel_flag.deleted = '0' AND eform_submission__field_vessel_flag.langcode = eform_submission_field_data.langcode)
			WHERE (( (eform_submission_field_data.type IN  ('', '', '', 'certificate')) AND eform_submission__field_vessel_flag.field_vessel_flag_value = 'JP' ))
			GROUP BY eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag_field_vessel_flag_value
			ORDER BY eform_submission_field_data_moderation_state ASC
			LIMIT 10 OFFSET 0) t) t1")->fetchAll();
		$chinese = db_query("select 'Chinese' as Country,
			sum(approved) as Approved, sum(draft) as Draft, sum(in_progress) as 'In Progress',sum(pending_approval) as 'Pending Approval',sum(rejected) as Rejected
			from (
			select eform_submission__field_vessel_flag_field_vessel_flag_value,
			case when eform_submission_field_data_moderation_state = 'approved' then eform_submission_field_data_created end as Approved,
			case when eform_submission_field_data_moderation_state = 'draft' then eform_submission_field_data_created end as draft,
			case when eform_submission_field_data_moderation_state = 'in_progress' then eform_submission_field_data_created end as in_progress,
			case when eform_submission_field_data_moderation_state = 'pending_approval' then eform_submission_field_data_created end as pending_approval,
			case when eform_submission_field_data_moderation_state = 'rejected' then eform_submission_field_data_created end as rejected
			from (
			SELECT eform_submission_field_data.moderation_state AS eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag.field_vessel_flag_value AS eform_submission__field_vessel_flag_field_vessel_flag_value, COUNT(eform_submission_field_data.created) AS eform_submission_field_data_created, MIN(eform_submission_field_data.eid) AS eid
			FROM
			eform_submission_field_data eform_submission_field_data
			LEFT JOIN eform_submission__field_vessel_flag eform_submission__field_vessel_flag ON eform_submission_field_data.eid = eform_submission__field_vessel_flag.entity_id AND (eform_submission__field_vessel_flag.deleted = '0' AND eform_submission__field_vessel_flag.langcode = eform_submission_field_data.langcode)
			WHERE (( (eform_submission_field_data.type IN  ('', '', '', 'certificate')) AND eform_submission__field_vessel_flag.field_vessel_flag_value = 'CN' ))
			GROUP BY eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag_field_vessel_flag_value
			ORDER BY eform_submission_field_data_moderation_state ASC
			LIMIT 10 OFFSET 0) t) t1")->fetchAll();
		$korean = db_query("select 'Korean' as Country,
			sum(approved) as Approved, sum(draft) as Draft, sum(in_progress) as 'In Progress',sum(pending_approval) as 'Pending Approval',sum(rejected) as Rejected
			from (
			select eform_submission__field_vessel_flag_field_vessel_flag_value,
			case when eform_submission_field_data_moderation_state = 'approved' then eform_submission_field_data_created end as Approved,
			case when eform_submission_field_data_moderation_state = 'draft' then eform_submission_field_data_created end as draft,
			case when eform_submission_field_data_moderation_state = 'in_progress' then eform_submission_field_data_created end as in_progress,
			case when eform_submission_field_data_moderation_state = 'pending_approval' then eform_submission_field_data_created end as pending_approval,
			case when eform_submission_field_data_moderation_state = 'rejected' then eform_submission_field_data_created end as rejected
			from (
			SELECT eform_submission_field_data.moderation_state AS eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag.field_vessel_flag_value AS eform_submission__field_vessel_flag_field_vessel_flag_value, COUNT(eform_submission_field_data.created) AS eform_submission_field_data_created, MIN(eform_submission_field_data.eid) AS eid
			FROM
			eform_submission_field_data eform_submission_field_data
			LEFT JOIN eform_submission__field_vessel_flag eform_submission__field_vessel_flag ON eform_submission_field_data.eid = eform_submission__field_vessel_flag.entity_id AND (eform_submission__field_vessel_flag.deleted = '0' AND eform_submission__field_vessel_flag.langcode = eform_submission_field_data.langcode)
			WHERE (( (eform_submission_field_data.type IN  ('', '', '', 'certificate')) AND eform_submission__field_vessel_flag.field_vessel_flag_value IN ('KR', 'KP') ))
			GROUP BY eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag_field_vessel_flag_value
			ORDER BY eform_submission_field_data_moderation_state ASC
			LIMIT 10 OFFSET 0) t) t1")->fetchAll();
		$other = db_query("select 'Other' as Country,
			sum(approved) as Approved, sum(draft) as Draft, sum(in_progress) as 'In Progress',sum(pending_approval) as 'Pending Approval',sum(rejected) as Rejected
			from (
			select eform_submission__field_vessel_flag_field_vessel_flag_value,
			case when eform_submission_field_data_moderation_state = 'approved' then eform_submission_field_data_created end as Approved,
			case when eform_submission_field_data_moderation_state = 'draft' then eform_submission_field_data_created end as draft,
			case when eform_submission_field_data_moderation_state = 'in_progress' then eform_submission_field_data_created end as in_progress,
			case when eform_submission_field_data_moderation_state = 'pending_approval' then eform_submission_field_data_created end as pending_approval,
			case when eform_submission_field_data_moderation_state = 'rejected' then eform_submission_field_data_created end as rejected
			from (
			SELECT eform_submission_field_data.moderation_state AS eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag.field_vessel_flag_value AS eform_submission__field_vessel_flag_field_vessel_flag_value, COUNT(eform_submission_field_data.created) AS eform_submission_field_data_created, MIN(eform_submission_field_data.eid) AS eid
			FROM
			eform_submission_field_data eform_submission_field_data
			LEFT JOIN eform_submission__field_vessel_flag eform_submission__field_vessel_flag ON eform_submission_field_data.eid = eform_submission__field_vessel_flag.entity_id AND (eform_submission__field_vessel_flag.deleted = '0' AND eform_submission__field_vessel_flag.langcode = eform_submission_field_data.langcode)
			WHERE (( (eform_submission_field_data.type IN  ('', '', '', 'certificate')) AND eform_submission__field_vessel_flag.field_vessel_flag_value NOT IN ('IN', 'KE', 'CN', 'KR', 'KP') ))
			GROUP BY eform_submission_field_data_moderation_state, eform_submission__field_vessel_flag_field_vessel_flag_value
			ORDER BY eform_submission_field_data_moderation_state ASC
			LIMIT 10 OFFSET 0) t) t1")->fetchAll();
		return new JsonResponse(Array($india[0], $chinese[0], $japanese[0], $korean[0], $other[0]) );
	}
	
	function monthSort($a,$b) {
      $monthA = date_parse($a->Month);
	  $monthB = date_parse($b->Month);
	  if($monthA == $monthB){
	    return 0;
	  }
	  return ($monthA > $monthB) ? 1 : -1;
	}
	
	public function getRoyaltyDueByMonth() {
		$start = date('Y-m-d');
	  $end = date('Y-m-01', strtotime('-5 months'));
	  
	  $invoice_dues = db_query("select eform_submission__field_invoice_date_field_invoice_date_valu as Month, SUM(Paid) as Paid, SUM(Unpaid) as Unpaid
                      from(
                        select eform_submission__field_invoice_date_field_invoice_date_valu,
                        case when eform_submission__field_payment_stae_field_payment_stae_valu = 'Paid' then eform_submission__field_converted_amount_field_converted_amo end as Paid,
                        case when eform_submission__field_payment_stae_field_payment_stae_valu = 'Unpaid' then eform_submission__field_converted_amount_field_converted_amo end as Unpaid
                        from(
                          SELECT eform_submission__field_payment_stae.field_payment_stae_value AS eform_submission__field_payment_stae_field_payment_stae_valu, monthname(eform_submission__field_invoice_date.field_invoice_date_value) AS eform_submission__field_invoice_date_field_invoice_date_valu, SUM(eform_submission__field_converted_amount.field_converted_amount_value) AS eform_submission__field_converted_amount_field_converted_amo, MIN(eform_submission_field_data.eid) AS eid
                          FROM
                          eform_submission_field_data eform_submission_field_data
                          LEFT JOIN eform_submission__field_payment_stae eform_submission__field_payment_stae ON eform_submission_field_data.eid = eform_submission__field_payment_stae.entity_id AND (eform_submission__field_payment_stae.deleted = '0')
                          LEFT JOIN eform_submission__field_converted_amount eform_submission__field_converted_amount ON eform_submission_field_data.eid = eform_submission__field_converted_amount.entity_id AND (eform_submission__field_converted_amount.deleted = '0')
                          LEFT JOIN eform_submission__field_invoice_date eform_submission__field_invoice_date ON eform_submission_field_data.eid = eform_submission__field_invoice_date.entity_id AND (eform_submission__field_invoice_date.deleted = '0')
                          WHERE (( (eform_submission_field_data.type IN  ('invoice')) AND eform_submission__field_invoice_date.field_invoice_date_value BETWEEN '$end' AND '$start' ))
                          GROUP BY eform_submission__field_payment_stae_field_payment_stae_valu, eform_submission__field_invoice_date_field_invoice_date_valu
                        ) t1
	                 ) t2 group by eform_submission__field_invoice_date_field_invoice_date_valu order by str_to_date(eform_submission__field_invoice_date_field_invoice_date_valu ,'%M') asc")->fetchAll();
	  usort($invoice_dues,array($this,'monthSort'));
	  return new JsonResponse($invoice_dues);
	}
	
	public function branchUpdateBatch(){
	  $fp = fopen("sites/default/files/branch_update.csv",'r');
	  while ($line = fgetcsv($fp)) {
	    $matchedEntity = entity_load('eform_submission', $line[0]);
	    $branchField = $matchedEntity->get('field_certificate_branch');
	    $branchField->setValue($line[2]);
	    $res[$line[0]]=$matchedEntity->save();
	  }
	  return new JsonResponse($res);
	}
	
	public function getAdminJobsByMonth() {
	  $start = date('Y-m-d');
	  $end = date('2016-11-01');
	  
	  $res = db_query("SELECT substring(eform_submission__field_date_of_inspection.field_date_of_inspection_value, 6,2) as month, COUNT(eform_submission_field_data.eid) AS count
		FROM
		{eform_submission_field_data} eform_submission_field_data
		LEFT JOIN {eform_submission__field_date_of_inspection} eform_submission__field_date_of_inspection ON eform_submission_field_data.eid = eform_submission__field_date_of_inspection.entity_id AND (eform_submission__field_date_of_inspection.deleted = '0' AND eform_submission__field_date_of_inspection.langcode = eform_submission_field_data.langcode)
		LEFT JOIN {eform_submission__field_service_engineer} eform_submission__field_service_engineer ON eform_submission_field_data.eid = eform_submission__field_service_engineer.entity_id AND (eform_submission__field_service_engineer.deleted = '0' AND eform_submission__field_service_engineer.langcode = eform_submission_field_data.langcode)
		WHERE (( (eform_submission_field_data.type IN ('certificate')) AND eform_submission_field_data.moderation_state IN ('approved') AND eform_submission__field_date_of_inspection.field_date_of_inspection_value BETWEEN '$end' AND '$start'))
		GROUP BY substring(eform_submission__field_date_of_inspection.field_date_of_inspection_value, 6,2)")->fetchAll();
		return new JsonResponse($res);
	}
}