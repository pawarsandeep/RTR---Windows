<?php
namespace Drupal\shm_mmr;

use Dompdf\Dompdf as DompdfLib;
use \Drupal\File\Entity\File;
class PDFGenerator{
  protected $dompdf;
  function __construct() {
    $this->dompdf = new DompdfLib(array('enable_html5_parser' => TRUE,'enable_remote' => TRUE,'default_paper_size' => 'a4',));
    $this->dompdf->setBaseHost(\Drupal::request()->getHttpHost());
    $this->dompdf->setProtocol(\Drupal::request()->getScheme() . '://');
  }

  /**
   * @param string $html
   * @param string $dir
   * @param string $fileName
   * @return \Drupal\file\FileInterface|false
   */
  function generatePdf($html, $dir, $fileName){

    $this->dompdf->loadHtml($html);
    $this->dompdf->render();
    $pdf = $this->dompdf->output();

    /**
     * $file = File::create(array(
     *  'uid' => 1,
     *  'file_name' => $fileName,
     *  'uri' => $dir . $fileName,
     *  'status' => 1,
     * ));
     * $file->save();
     * $filePath = $file->getFileUri();
     * file_prepare_directory(dirname($filePath),FILE_CREATE_DIRECTORY|FILE_MODIFY_PERMISSIONS);
     * file_put_contents($filePath,$pdf);
     * $file->save();
     */
    $filePath = $dir . $fileName . '.pdf';
    $fileSystem = \Drupal::service('file_system');
    if(!file_exists($filePath)){
      file_prepare_directory(dirname($filePath), FILE_MODIFY_PERMISSIONS|FILE_CREATE_DIRECTORY);
      //$fileSystem->mkdir(dirname($filePath));
    }
    $file = file_save_data($pdf,$filePath,FILE_EXISTS_RENAME);
    return($file);

  }

}