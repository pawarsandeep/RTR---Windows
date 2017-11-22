<?php
/**
 * Created by PhpStorm.
 * User: Sandeep
 * Date: 26-09-2017
 * Time: 12:59 PM
 */

namespace Drupal\shm_ajs\normalizer;

use Drupal\serialization\Normalizer\NormalizerBase;
use Symfony\Component\Serializer\Normalizer\DenormalizerInterface;

/**
 * Class JsonDenormalizer
 * @package Drupal\shm_ajs\normalizer
 */
class JsonDenormalizer extends NormalizerBase implements DenormalizerInterface{
  /**
   * The interface or class that this Normalizer supports.
   * @var array
   */
  protected $supportedInterfaceOrClass = array(__CLASS__);

  /**
   * {@inheritdoc}
   */
  public function normalize($object, $format = NULL, array $context = array()) {
    return parent::normalize($object,$format,$context);
  }

  /**
   * {@inheritdoc}
   */
  public function denormalize($data, $class, $format = NULL, array $context = array()) {
    return $data;
  }
}
