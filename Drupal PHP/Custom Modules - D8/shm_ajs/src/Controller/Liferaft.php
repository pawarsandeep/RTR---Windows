<?php
/**
 * Created by PhpStorm.
 * User: Sandeep
 * Date: 7/19/2017
 * Time: 3:42 AM
 */

namespace Drupal\shm_ajs\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Entity\EntityFormBuilder;
use Drupal\Core\Entity\Entity\EntityFormMode;

class Liferaft extends ControllerBase
{
    public function newJob()
    {
        $formMode = entity_get_form_display('job_entity','liferaft_service_job','default');
        $thirdPartySettings = $formMode->getThirdPartySettings('field_group');
        $fieldDefinitions = $formMode->get('fieldDefinitions');
        $fields=null;
        foreach($thirdPartySettings as $groupKey=>$groupSettings)
        {
            foreach ($groupSettings['children'] as $fieldName)
            {
                $fields[$groupKey][$fieldName] = $fieldDefinitions[$fieldName];
            }
        }
        $form = array('form'=> array(
            '#theme' => 'liferaft_new_job',
            '#fields' => $fields,
        ),);
        $form['#attached']['library'][] = 'shm_ajs/liferaft_job';
        $form['#attached']['drupalSettings']['shm_ajs']['csrf_token'] = \Drupal::csrfToken()->get('rest');
        return ($form);
    }

    public function editJob()
    {
        // drupal_set_message("Te4534st");
        $form = array('form'=> array(
            '#theme' => 'liferaft_new_job',
        ),);
        $form['#attached']['library'][] = 'shm_ajs/liferaft_job';
        $form['#attached']['drupalSettings']['shm_ajs']['csrf_token'] = \Drupal::csrfToken()->get('rest');
        return ($form);
    }
}