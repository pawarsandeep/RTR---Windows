<?php
/**
 * Created by PhpStorm.
 * User: Nikhil
 * Date: 14-Nov-17
 * Time: 5:09 PM
 */
namespace Drupal\shm_ajs\Access ;
use \Drupal\Core\Routing\Access\AccessInterface ;
use \Drupal\Core\Session\AccountInterface ;
use \Drupal\Core\Access\AccessResult ;

class CustomAccessCheck implements AccessInterface {

    public function access(AccountInterface $account){
        $role= $account->getRoles();
        $tid= $account->id();
        $user =  \Drupal::service('entity_type.manager')->getStorage('user')->load($tid);
        $jobid = explode("/", $_SERVER['REQUEST_URI'])[2];

        switch ($role[1]){
            case 'liferaft_service_engineer':

                if ($user->get('field_liferaft_branch')->getString() == \Drupal\shm_services_entity\Entity\JobSubmission::load($jobid)->get('field_branch')->getString() and (\Drupal\shm_services_entity\Entity\JobSubmission::load($jobid)->get('moderation_state')->getString()=='draft' or \Drupal\shm_services_entity\Entity\JobSubmission::load($jobid)->get('moderation_state')->getString() == 'rejected') ){
                    return AccessResult::allowed();
                }
                break;
            case 'liferaft_branch_manager':

                if ($user->get('field_liferaft_branch')->getString() == \Drupal\shm_services_entity\Entity\JobSubmission::load($jobid)->get('field_branch')->getString() ){
                    return AccessResult::allowed();
                }
                break;

            case 'liferaft_incharge':

                if ($user->get('field_liferaft_branch')->getString() == \Drupal\shm_services_entity\Entity\JobSubmission::load($jobid)->get('field_branch')->getString() ){
                    return AccessResult::allowed();
                }
                break;
				
            case 'shm_admin':
                return AccessResult::allowed();
				break;

            case 'administrator':
                return AccessResult::allowed();
				break;
        }
        return AccessResult::forbidden();
    }
}